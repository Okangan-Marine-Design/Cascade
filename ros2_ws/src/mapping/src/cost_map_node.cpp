#include <string>
#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "cascade_msgs/msg/voxel_grid.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "bonxai/bonxai.hpp"
#include "bonxai/serialization.hpp"
#include <tf2/LinearMath/Vector3.h>

using std::placeholders::_1;

class CostmapNode : public rclcpp::Node
{
    public:
        CostmapNode() 
        : Node("motion_planner_node"), costmap(0.2){ 

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

        void inflateObstacles(const float& data, const Bonxai::CoordT& coord){
            if(data!=1.0)return;
            Bonxai::Point3D pos = costmap.coordToPos(coord);
            auto accessor= costmap.createAccessor();
            int range=2;
            for(int x=-range;x<=range;x++){
                for(int y=-range;y<=range;y++){
                    for(int z=-range;z<=range;z++){
                        if(abs(z)==range || abs(y)==range || abs(x)==range){
                            accessor.setCellOn(costmap.posToCoord(pos.x+x*costmap.resolution, pos.y+y*costmap.resolution, pos.z+z*costmap.resolution), -1.0);
                        }
                    }
                }
            }
        }

        void inflateMap(){
            if(loading || working)return;
            working=true;
            auto inflateObstaclesLambda = [this](const float& data, const Bonxai::CoordT& coord) {
                this->inflateObstacles(data, coord); // Call the member function using instance-specific data
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
            auto g=Bonxai::Deserialize<float>(ifile, info);
            costmap=std::move(g);
            loading=false;
            return true;
        }

        class node{
            public:
            float x,y,z,dist,cost;
            bool operator==(const node& n) const{
                return abs(x - n.x)<0.1 && abs(y - n.y)<0.1 && abs(z - n.z)<0.1;
            }
            bool operator<(const node& n) const{
                return (cost+dist)<(n.cost+n.dist);
            }

        };
        
        struct nodeHash {
            std::size_t operator()(const node& s) const {
                std::size_t hashX = std::hash<float>()(s.x);
                std::size_t hashY = std::hash<float>()(s.y);
                std::size_t hashZ = std::hash<float>()(s.z);

                // Combine hash values using bitwise XOR and multiplication
                return hashX ^ (hashY << 1) ^ (hashZ << 2);
            }
        };
        
        struct nodeCompare {
            bool operator()(const node& n1, const node& n2) const { return (n1.cost+n1.dist)>(n2.cost+n2.dist); }
        };

        float dist(node n1, node n2){
            return pow(
                        pow(n1.x-n2.x,2)+
                        pow(n1.y-n2.y,2)+
                        pow(n1.z-n2.z,2)
                        ,1.0/2.0);
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
        Bonxai::VoxelGrid<float> costmap;
        rclcpp::Publisher<cascade_msgs::msg::VoxelGrid>::SharedPtr gridPublisher;
        rclcpp::Subscription<cascade_msgs::msg::VoxelGrid>::SharedPtr costmap_subscription;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CostmapNode>());
    rclcpp::shutdown();
    return 0;
}
