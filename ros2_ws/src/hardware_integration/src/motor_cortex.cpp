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

class MotorCortexNode : public rclcpp::Node
{
    public:
        MotorCortexNode() 
        : Node("motor_cortex_node"){ 
        subscription_ =
            this->create_subscription<geometry_msgs::msg::Pose>("/current_goal_pose", 10, std::bind(&MotorCortexNode::goal_pose_callback, this, _1));
            publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/current_goal_status", 10);
        }
    private:
        void goal_pose_callback(geometry_msgs::msg::Pose msg){
            auto message = cascade_msgs::msg::Status();
            message.status = 0;
            publisher_->publish(message);
        }

        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr subscription_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr publisher_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotorCortexNode>());
  rclcpp::shutdown();
  return 0;
}
