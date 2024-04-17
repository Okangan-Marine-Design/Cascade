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
        bool inserted=false;
        MotionPlannerNode() 
        : Node("motion_planner_node"), costmap(0.2){ 

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
            replaceCostmap(msg);
            calculatePath();
        }

        bool replaceCostmap(cascade_msgs::msg::VoxelGrid msg){
            if(loading || searching)return false;
            loading=true;
            std::string serialized_data(msg.data.begin(), msg.data.end());

            std::istringstream ifile(serialized_data, std::ios::binary);
    
            char header[256];
            ifile.getline(header, 256);
            Bonxai::HeaderInfo info = Bonxai::GetHeaderInfo(header);
            auto g=Bonxai::Deserialize<float>(ifile, info);
            costmap=std::move(g);
            loading=false;
            inserted=true;
            return true;
        }

        bool checkNewObstacles(){
            return true;//assume always needs to be recalcualted for now
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

        void clear_path_nodes(std::vector<node> pathNodes){
            auto accessor=costmap.createAccessor();
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

        std::vector<node> aStarSearch(){
            std::vector<node> result;
            auto accessor=costmap.createAccessor();
            //RCLCPP_INFO(this->get_logger(), "starting a*");
            node start,goal;
            start.x = currentPoseMsg.pose.position.x;
            start.y = currentPoseMsg.pose.position.y;
            start.z = -currentPoseMsg.pose.position.z;
            //start.x=0;
            //start.y=0;
            //start.z=0;
            goal.x = currentEndPoseMsg.pose.position.x;
            goal.y = currentEndPoseMsg.pose.position.y;
            goal.z = -currentEndPoseMsg.pose.position.z;//why does this need to be negative i have no clue

            std::vector<node> surrounding;
            std::unordered_set<node, nodeHash> checked;
            std::unordered_map<node, node, nodeHash> previous;
            std::priority_queue<node, std::vector<node>, nodeCompare> openSet;
            start.cost=0;
            start.dist=pow(dist(start,goal),2);
            openSet.push(start);
            checked.insert(start);

            while(!openSet.empty()){
                node current=openSet.top();
                if(current==goal){

                    //RCLCPP_INFO(this->get_logger(), "found goal");
                    while(previous.count(current)>0){
                        result.push_back(current);
                        current=previous[current];
                    }
                    //std::string logMessage = "returning with " + std::to_string(result.size()) + " path nodes";
                    //RCLCPP_INFO(this->get_logger(), logMessage.c_str());
                    return result;
                }
                openSet.pop();
                //checked.erase(current);
                //creating set of neighboring nodes
                for(int x=-1;x<=1;x++){
                    for(int y=-1;y<=1;y++){
                        for(int z=-1;z<=1;z++){
                            node tempNode;
                            tempNode.x=current.x+x*costmap.resolution*0.75;
                            tempNode.y=current.y+y*costmap.resolution*0.75;
                            tempNode.z=current.z+z*costmap.resolution*0.75;
                            Bonxai::CoordT coord = costmap.posToCoord(tempNode.x, tempNode.y, tempNode.z);
                            float* value_ptr = accessor.value( coord );
                            bool valid=false;
                            if(value_ptr==nullptr)valid=true;
                            else if(*value_ptr==0.0)valid=true;
                            if(valid){
                                if(checked.count(tempNode)>0){
                                    tempNode=*checked.find(tempNode);
                                }else
                                    tempNode.cost=std::numeric_limits<float>::infinity();
                                surrounding.push_back(tempNode);
                            }
                        }
                    }
                }
                for(node n: surrounding){
                    float score=current.cost+dist(current,n);
                    if(score<n.cost){
                        previous[n]=current;
                        n.cost=score;
                        n.dist=pow(dist(n,goal),2);
                        if(checked.count(n)==0){
                            checked.insert(n);
                            openSet.push(n);
                            //result.push_back(n);
                        }
                    }
                }
                surrounding.clear();
                //std::string logMessage = "explored nodes " + std::to_string(checked.size());
                //RCLCPP_INFO(this->get_logger(), logMessage.c_str());
            }
            return result;
        }

        std::vector<node> bestFirstSearch(){
            std::vector<node> result;
            auto accessor=costmap.createAccessor();
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
                            tempNode.x=current.x+x*costmap.resolution;
                            tempNode.y=current.y+y*costmap.resolution;
                            tempNode.z=current.z+z*costmap.resolution;
                            if(visited.count(tempNode)==0){
                                surrounding.push_back(tempNode);
                            }
                        }
                    }
                }
                //std::string logMessage = "There are " + std::to_string(surrounding.size()) + " valid neighbors";
                //RCLCPP_INFO(this->get_logger(), logMessage.c_str());
                node best;
                float minDist=std::numeric_limits<float>::infinity();
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
                visited.insert(best);
                result.push_back(best);
            }
            //push back results (back track from goal)
            return result;
        }

        std::queue<gpose> calculatePath(){//best first search at the moment, if it turns out to be too slow the algo will be changed
            std::queue<gpose> result;
            if(searching || loading)return result;
            auto accessor=costmap.createAccessor();
            searching=true;
            std::vector<node> path=aStarSearch();
            for(node n: path){
                accessor.setValue(costmap.posToCoord(n.x,n.y,n.z), 2.0);
            }
            //insert the path nodes into the costmap and publish under /path_grid
            publishVoxelGrid();
            clear_path_nodes(path);
            searching=false;
            return result;
        }

        bool loading=false, haveGoal=false, searching=false;
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
