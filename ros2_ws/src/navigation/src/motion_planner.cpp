#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "cascade_msgs/msg/voxel_grid.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "bonxai/bonxai.hpp"
#include "bonxai/serialization.hpp"
#include <queue>

using std::placeholders::_1;
typedef cascade_msgs::msg::GoalPose gpose;

class MotionPlannerNode : public rclcpp::Node
{
    public:
        MotionPlannerNode() 
        : Node("motion_planner_node"), costmap(0.2), accessor(costmap){ 

        end_pose_subscription = this->create_subscription<geometry_msgs::msg::Pose>("/end_goal_pose", 10, std::bind(&MotionPlannerNode::end_pose_callback, this, _1));

        goal_pose_publisher = this->create_publisher<cascade_msgs::msg::GoalPose>("/current_goal_pose", 10);
        status_publisher = this->create_publisher<cascade_msgs::msg::Status>("/end_goal_status", 10);

        status_subscription = this->create_subscription<cascade_msgs::msg::Status>("/current_goal_status", 10, std::bind(&MotionPlannerNode::goal_status_callback, this, _1));
        costmap_subscription = this->create_subscription<cascade_msgs::msg::VoxelGrid>("/voxel_grid", 10, std::bind(&MotionPlannerNode::costmap_callback, this, _1));

        current_pose_subscription = this->create_subscription<geometry_msgs::msg::PoseStamped>("/pose", 10, std::bind(&MotionPlannerNode::current_pose_callback, this, _1));
    }
    private:
        void end_pose_callback(geometry_msgs::msg::Pose msg){
            //set end goal pose
            currentEndPoseMsg=msg;
            calculatePath();//when we get a new end pose calculate a new path
        }
        void current_pose_callback(geometry_msgs::msg::PoseStamped msg){
            //set current pose
            currentPoseMsg=msg;
            if(checkNewObstacles())//check if there are any new obstacles from curr pose to current goal
                calculatePath();
        }

        void goal_status_callback(cascade_msgs::msg::Status msg){
            //receives success, in progress, or failure from the motor cortex
            //if successfully reached its goal, publish next goal pose in the queue
        }

        void costmap_callback(cascade_msgs::msg::VoxelGrid msg){
            if(replaceCostmap(msg))//if the costmap is successfully replaced
                if(checkNewObstacles())//check if there are any new obstacles
                    calculatePath();//recalculate path only if there are new obstacles between the auv and the goal
        }

        bool replaceCostmap(cascade_msgs::msg::VoxelGrid msg){
            if(loading)return false;
            loading=true;
            std::string serialized_data(msg.data.begin(), msg.data.end());

            std::istringstream ifile(serialized_data, std::ios::binary);
    
            char header[256];
            ifile.getline(header, 256);
            Bonxai::HeaderInfo info = Bonxai::GetHeaderInfo(header);
            costmap=std::move(Bonxai::Deserialize<float>(ifile, info));
            loading=false;
            return true;
        }

        bool checkNewObstacles(){
            return true;//assume always needs to be recalcualted for now
        }
        struct node{
            float x,y,z,dist;
        };
        float dist(node n1, node n2){
            return pow(
                        pow(n1.x-n2.x,2)+
                        pow(n1.y-n2.y,2)+
                        pow(n1.z-n2.z,2)
                        ,1.0/2.0);
        }
        void clear_path_nodes(std::vector<node> pathNodes){
            for(node n: pathNodes){
                Bonxai::CoordT coord = costmap.posToCoord(n.x, n.y, n.z);
                accessor.setCellOff( coord );
                //accessor.setValue( coord, 0.0 );
            }
        }
        std::vector<node> bestFirstSearch(){
            std::vector<node> result;
            //start node -> check every node around 
            //for each valid node (not an obstacle) check the straight line distance to the goal
            //add current node to list of path nodes
            //make best surrounding node current node
            //repeat until at goal
            //
            //set each path node as 2.0 and publish for visualization
            node current,goal;
            current.x = currentPoseMsg.pose.position.x;
            current.y = currentPoseMsg.pose.position.y;
            current.z = currentPoseMsg.pose.position.z;
            goal.x = currentEndPoseMsg.position.x;
            goal.y = currentEndPoseMsg.position.y;
            goal.z = currentEndPoseMsg.position.z;
            std::vector<node> surrounding, path;
            while(dist(current,goal)>costmap.resolution*1.5){
                for(int x=-1;x<=1;x++){
                    for(int y=-1;y<=1;y++){
                        for(int z=-1;z<=1;z++){
                            node tempNode;
                            tempNode.x=current.x+x*costmap.resolution;
                            tempNode.y=current.y+y*costmap.resolution;
                            tempNode.z=current.z+z*costmap.resolution;
                            surrounding.push_back(tempNode);
                        }
                    }
                }
                node best;
                float minDist=std::numeric_limits<float>::infinity();;
                for(node n: surrounding){
                    Bonxai::CoordT coord = costmap.posToCoord(n.x, n.y, n.z);
                    float* value_ptr = accessor.value( coord );
                    bool valid=false;
                    if(value_ptr==nullptr)valid=true;
                    else if(*value_ptr==2.0)valid=true;
                    if(valid && dist(n,goal)<minDist){//if the cell is unoccupied and it is the best cell so far
                        best=n;
                        minDist=dist(n,goal);
                    }
                }
                current=best;
                result.push_back(best);
            }
            return result;
        }

        std::queue<gpose> calculatePath(){//best first search at the moment, if it turns out to be too slow the algo will be changed
            std::queue<gpose> result;
            bestFirstSearch();
            //insert the path nodes into the costmap and publish under /path_grid
            return result;
        }

        bool loading=false;
        Bonxai::VoxelGrid<float>::Accessor accessor;
        Bonxai::VoxelGrid<float> costmap;
        geometry_msgs::msg::PoseStamped currentPoseMsg;
        geometry_msgs::msg::Pose currentEndPoseMsg;
        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr end_pose_subscription;
        rclcpp::Subscription<cascade_msgs::msg::VoxelGrid>::SharedPtr costmap_subscription;
        rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr  current_pose_subscription;
        rclcpp::Subscription<cascade_msgs::msg::Status>::SharedPtr status_subscription;
        rclcpp::Publisher<cascade_msgs::msg::GoalPose>::SharedPtr goal_pose_publisher;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotionPlannerNode>());
  rclcpp::shutdown();
  return 0;
}
