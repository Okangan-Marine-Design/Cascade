#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "geometry_msgs/msg/pose.hpp"

using std::placeholders::_1;

class MotionPlannerNode : public rclcpp::Node
{
    public:
        MotionPlannerNode() 
        : Node("motion_planner_node"){ 

        pose_subscription_ = this->create_subscription<geometry_msgs::msg::Pose>("/end_goal_pose", 10, std::bind(&MotionPlannerNode::end_pose_callback, this, _1));

        pose_publisher_ = this->create_publisher<cascade_msgs::msg::GoalPose>("/current_goal_pose", 10);
        status_publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/end_goal_status", 10);

        status_subscription_ = this->create_subscription<cascade_msgs::msg::Status>("/current_goal_status", 10, std::bind(&MotionPlannerNode::goal_status_callback, this, _1));
    }
    private:
        void end_pose_callback(geometry_msgs::msg::Pose msg){
        
        }

        void goal_status_callback(cascade_msgs::msg::Status msg){
            
        }

        void calculatePath(){
            
        }

        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr pose_subscription_;
        rclcpp::Subscription<cascade_msgs::msg::Status>::SharedPtr status_subscription_;
        rclcpp::Publisher<cascade_msgs::msg::GoalPose>::SharedPtr pose_publisher_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotionPlannerNode>());
  rclcpp::shutdown();
  return 0;
}
