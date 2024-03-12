#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "geometry_msgs/msg/pose.hpp"

using std::placeholders::_1;

class NavigatorNode : public rclcpp::Node
{
    public:
        NavigatorNode() 
        : Node("navigator_node"){ 
        command_subscription_ =
            this->create_subscription<cascade_msgs::msg::MovementCommand>("/movement_command", 10, std::bind(&NavigatorNode::movement_command_callback, this, _1));

            pose_publisher_ = this->create_publisher<geometry_msgs::msg::Pose>("/end_goal_pose", 10);
            status_publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/movement_command_status", 10);

        status_subscription_ = 
            this->create_subscription<cascade_msgs::msg::Status>("/end_goal_status", 10, std::bind(&NavigatorNode::goal_status_callback, this, _1));
        }
    private:
        void movement_command_callback(cascade_msgs::msg::MovementCommand msg){
        
        }
        void goal_status_callback(cascade_msgs::msg::Status msg){
        
        }

        rclcpp::Subscription<cascade_msgs::msg::MovementCommand>::SharedPtr command_subscription_;
        rclcpp::Subscription<cascade_msgs::msg::Status>::SharedPtr status_subscription_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher_;
        rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr pose_publisher_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NavigatorNode>());
  rclcpp::shutdown();
  return 0;
}
