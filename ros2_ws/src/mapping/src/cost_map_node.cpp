#include <string>
#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/classes.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "cascade_msgs/msg/voxel_grid.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "bonxai/bonxai.hpp"
#include "bonxai/serialization.hpp"
#include <tf2/LinearMath/Vector3.h>
#include <queue>
#include <vector>

using std::placeholders::_1;

class CostmapNode : public rclcpp::Node
{
    public:
        CostmapNode() 
        : Node("costmap_node"), costmap(0.2){ 

        costmap_subscription = this->create_subscription<cascade_msgs::msg::VoxelGrid>("/voxel_grid", 10, std::bind(&CostmapNode::costmap_callback, this, _1));
        gridPublisher = this->create_publisher<cascade_msgs::msg::VoxelGrid>("/costmap_grid", 10);
    }
    private:
        void costmap_callback(cascade_msgs::msg::VoxelGrid msg){
            if(loading || working)return;
            replaceCostmap(msg);
            inflateMap();
            publishVoxelGrid();
        }

        void inflateMap(){
            if(loading || working)return;
            working=true;
            auto accessor= costmap.createAccessor();
            auto inflateObstaclesLambda = [this, &accessor](const std::array<int,2>& data, const Bonxai::CoordT& coord) {
                if(accessor.value(coord)==nullptr)return;
                Bonxai::Point3D pos = costmap.coordToPos(coord);
                int range=3;
                for(int x=-range;x<=range;x++){
                    for(int y=-range;y<=range;y++){
                        for(int z=-range;z<=range;z++){
                            if(abs(z)==range || abs(y)==range || abs(x)==range){
                                accessor.setCellOn(
                                        costmap.posToCoord(pos.x+x*costmap.resolution, pos.y+y*costmap.resolution, pos.z+z*costmap.resolution), 
                                        {cascade_msgs::msg::Classes::INFLATED,100});
                            }
                        }
                    }
                }
            };
            costmap.forEachCell(inflateObstaclesLambda);
            working=false;
        }


        bool replaceCostmap(cascade_msgs::msg::VoxelGrid msg){
            if(loading || working)return false;
            loading=true;
            std::string serialized_data(msg.data.begin(), msg.data.end());

            std::istringstream ifile(serialized_data, std::ios::binary);
    
            char header[256];
            ifile.getline(header, 256);
            Bonxai::HeaderInfo info = Bonxai::GetHeaderInfo(header);
            auto g=Bonxai::Deserialize<std::array<int,2>>(ifile, info);
            costmap=std::move(g);
            loading=false;
            return true;
        }

        void publishVoxelGrid(){
            std::ostringstream ofile(std::ios::binary);
            Bonxai::Serialize(ofile, costmap);
            std::string s=ofile.str();
    
            cascade_msgs::msg::VoxelGrid gridMsg;

            std::vector<unsigned char> charVector(s.begin(), s.end());
            gridMsg.data=charVector;

            gridPublisher->publish(gridMsg);
        }

        bool loading=false, working=false;
        Bonxai::VoxelGrid<std::array<int,2>> costmap;
        rclcpp::Publisher<cascade_msgs::msg::VoxelGrid>::SharedPtr gridPublisher;
        rclcpp::Subscription<cascade_msgs::msg::VoxelGrid>::SharedPtr costmap_subscription;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CostmapNode>());
    rclcpp::shutdown();
    return 0;
}
