#include "rclcpp/rclcpp.hpp"
#include "robo_messages/srv/rgbd.hpp"
#include "robo_messages/srv/stereo.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <memory>
#include <string.h>
#include "main.hpp"
//TODO: import opencv

void stereo2rgbd_service_callback(const std::shared_ptr<robo_messages::srv::RGBD::Request> request,
    std::shared_ptr<robo_messages::srv::RGBD2::Response>      response){
    auto left=request->left;
    auto right=request->right;
    sensor_msgs::msg::Image result_rgb, result_depth;

    //do procressing
    response->rgb = result_rgb;
    response->depth = result_depth;
}
    
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("sensor_processing");
    
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sensor_processing node launched");
    //creation of RGBD data subscription and service
    rclcpp::Service<slam_messages::srv::RGBD>::SharedPtr service_rgbd = 
        node->create_service<slam_messages::srv::RGBD>("stereo2rgbd", &send_rgbd);

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}




