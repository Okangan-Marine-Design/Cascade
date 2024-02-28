#include "rclcpp/rclcpp.hpp"
#include "System.h"

void destination_subscription_callback(const robo_messages::msg::pos_estimate &msg){
    std::cout<<"got messgage from /destination";
    //run a* and create path using map data?
    //maybe this should just be in the map node?
    //find way to access or share octomap object over ROS communication
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("navigation");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created navigation node!");
        
    rclcpp::Subscription<package::msg::msg_type>::SharedPtr subscription=
    node->create_subscription<package::msg::msg_type>("/destination",10, &destination_subscription_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
