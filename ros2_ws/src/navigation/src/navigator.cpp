#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/srv/find_object.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>


using std::placeholders::_1;
using namespace std::chrono_literals;

class NavigatorNode : public rclcpp::Node
{
    public:
        NavigatorNode() 
        : Node("navigator_node"){ 
        command_subscription_ =
            this->create_subscription<cascade_msgs::msg::MovementCommand>("/movement_command", 10, std::bind(&NavigatorNode::movement_command_callback, this, _1));

            pose_publisher_ = this->create_publisher<cascade_msgs::msg::GoalPose>("/end_goal_pose", 10);
            status_publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/movement_command_status", 10);

            status_subscription_ = 
                this->create_subscription<cascade_msgs::msg::Status>("/end_goal_status", 10, std::bind(&NavigatorNode::goal_status_callback, this, _1));

        
            subNode = rclcpp::Node::make_shared("_navigator_client");
            client=subNode->create_client<cascade_msgs::srv::FindObject>("find_object");
            sendObjectRequest();
        }
    private:
        void movement_command_callback(cascade_msgs::msg::MovementCommand msg){
        
        }
        void goal_status_callback(cascade_msgs::msg::Status msg){
        
        }
        int sendObjectRequest(){
            auto request = std::make_shared<cascade_msgs::srv::FindObject::Request>();

            while (!client->wait_for_service(1s)) {
                if (!rclcpp::ok()) {
                    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
                    return 1;
                }
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
            }

            auto result = client->async_send_request(request);
            // Wait for the result.
            if (rclcpp::spin_until_future_complete(subNode, result) ==
                rclcpp::FutureReturnCode::SUCCESS)
            {
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "got object location");
            } else {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service find_object");
            }
            return 0;
        }
        geometry_msgs::msg::Quaternion quaternion_from_rpy(float roll, float pitch, float yaw) {
            // Convert roll, pitch, yaw angles to quaternion
            tf2::Quaternion quat;
            quat.setRPY(roll, pitch, yaw);

            // Convert tf2 Quaternion to ROS 2 Quaternion message
            geometry_msgs::msg::Quaternion quat_msg;
            tf2::convert(quat, quat_msg);

            return quat_msg;
        }

        rclcpp::Subscription<cascade_msgs::msg::MovementCommand>::SharedPtr command_subscription_;
        rclcpp::Subscription<cascade_msgs::msg::Status>::SharedPtr status_subscription_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher_;
        rclcpp::Publisher<cascade_msgs::msg::GoalPose>::SharedPtr pose_publisher_;
        rclcpp::Client<cascade_msgs::srv::FindObject>::SharedPtr client;
        std::shared_ptr<rclcpp::Node> subNode;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NavigatorNode>());
  rclcpp::shutdown();
  return 0;
}
