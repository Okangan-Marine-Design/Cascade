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
#include <unordered_map>

using std::placeholders::_1;
typedef cascade_msgs::msg::GoalPose gpose;

class MotionPlannerNode : public rclcpp::Node
{
    public:
        MotionPlannerNode() 
        : Node("motion_planner_node"), costmap(0.2), accessor(costmap){ 

        end_pose_subscription = this->create_subscription<cascade_msgs::msg::GoalPose>("/end_goal_pose", 10, std::bind(&MotionPlannerNode::end_pose_callback, this, _1));

        goal_pose_publisher = this->create_publisher<cascade_msgs::msg::GoalPose>("/current_goal_pose", 10);
        status_publisher = this->create_publisher<cascade_msgs::msg::Status>("/end_goal_status", 10);

        status_subscription = this->create_subscription<cascade_msgs::msg::Status>("/current_goal_status", 10, std::bind(&MotionPlannerNode::goal_status_callback, this, _1));
        costmap_subscription = this->create_subscription<cascade_msgs::msg::VoxelGrid>("/voxel_grid", 10, std::bind(&MotionPlannerNode::costmap_callback, this, _1));

        current_pose_subscription = this->create_subscription<geometry_msgs::msg::PoseStamped>("/pose", 10, std::bind(&MotionPlannerNode::current_pose_callback, this, _1));

        gridPublisher = this->create_publisher<cascade_msgs::msg::VoxelGrid>("/path_grid", 10);
    }
    private:
        void end_pose_callback(cascade_msgs::msg::GoalPose msg){
            //set end goal pose
            currentEndPoseMsg=msg;
            haveGoal=true;
            calculatePath();//when we get a new end pose calculate a new path
        }
        void current_pose_callback(geometry_msgs::msg::PoseStamped msg){
            //set current pose
            currentPoseMsg=msg;
            if(haveGoal && checkNewObstacles())//check if there are any new obstacles from curr pose to current goal
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
        class node{
            public:
            float x,y,z;
            bool operator==(const node& n) const{
                return x == n.x && y == n.y && z==n.z;
            }
        };
        
        struct nodeHash {
            std::size_t operator()(const node& s) const {
                return std::hash<float>()(s.x) ^ (std::hash<float>()(s.y) << 1);
            }
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

        void publishVoxelGrid(){//for visualization and debugging
            std::ostringstream ofile(std::ios::binary);
            Bonxai::Serialize(ofile, costmap);
            std::string s=ofile.str();
    
            cascade_msgs::msg::VoxelGrid gridMsg;

            std::vector<unsigned char> charVector(s.begin(), s.end());
            gridMsg.data=charVector;

            gridPublisher->publish(gridMsg);
        }

        std::vector<node> bestFirstSearch(){
            std::vector<node> result;
            if(searching)return result;
            searching=true;
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
            current.z = -currentPoseMsg.pose.position.z;
            goal.x = currentEndPoseMsg.pose.position.x;
            goal.y = currentEndPoseMsg.pose.position.y;
            goal.z = -currentEndPoseMsg.pose.position.z;//why does this need to be negative i have no clue
            std::vector<node> surrounding, path;
            std::unordered_set<node, nodeHash> visited;
            while(dist(current,goal)>costmap.resolution*1.5){
                for(int x=-1;x<=1;x++){
                    for(int y=-1;y<=1;y++){
                        for(int z=-1;z<=1;z++){
                            node tempNode;
                            tempNode.x=current.x+x*costmap.resolution*0.75;
                            tempNode.y=current.y+y*costmap.resolution*0.75;
                            tempNode.z=current.z+z*costmap.resolution*0.75;
                            if(visited.count(tempNode)==0){
                                surrounding.push_back(tempNode);
                            }
                        }
                    }
                }
                //std::string logMessage = "There are " + std::to_string(surrounding.size()) + " valid neighbors";
                //RCLCPP_INFO(this->get_logger(), logMessage.c_str());
                node best;
                float minDist=std::numeric_limits<float>::infinity();;
                for(node n: surrounding){
                    Bonxai::CoordT coord = costmap.posToCoord(n.x, n.y, n.z);
                    float* value_ptr = accessor.value( coord );
                    bool valid=false;
                    if(value_ptr==nullptr)valid=true;
                    if(valid && dist(n,goal)<=minDist){//if the cell is unoccupied and it is the best cell so far
                        best=n;
                        minDist=dist(n,goal);
                        //RCLCPP_INFO(this->get_logger(), "got a better option");
                    }
                }
                surrounding.clear();
                current=best;
                result.push_back(best);
                visited.insert(best);
            }
            searching=false;
            return result;
        }

        std::queue<gpose> calculatePath(){//best first search at the moment, if it turns out to be too slow the algo will be changed
            std::queue<gpose> result;
            std::vector<node> path=bestFirstSearch();
            for(node n: path){
                accessor.setValue(costmap.posToCoord(n.x,n.y,n.z), 2.0);
            }
            //insert the path nodes into the costmap and publish under /path_grid
            publishVoxelGrid();
            clear_path_nodes(path);
            return result;
        }

        bool loading=false, haveGoal=false, searching=false;
        Bonxai::VoxelGrid<float>::Accessor accessor;
        Bonxai::VoxelGrid<float> costmap;
        rclcpp::Publisher<cascade_msgs::msg::VoxelGrid>::SharedPtr gridPublisher;
        geometry_msgs::msg::PoseStamped currentPoseMsg;
        cascade_msgs::msg::GoalPose currentEndPoseMsg;
        rclcpp::Subscription<cascade_msgs::msg::GoalPose>::SharedPtr end_pose_subscription;
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
