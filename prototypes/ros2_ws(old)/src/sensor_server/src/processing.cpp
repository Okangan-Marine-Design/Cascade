#include "rclcpp/rclcpp.hpp"
#include "robo_messages/srv/rgbd.hpp"
#include "robo_messages/srv/stereo.hpp"
#include "robo_messages/srv/stereo2_rgbd.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <memory>
#include <string.h>
//TODO: import opencv

void stereo2rgbd_service_callback(const std::shared_ptr<robo_messages::srv::Stereo2RGBD::Request> request,
    std::shared_ptr<robo_messages::srv::Stereo2RGBD::Response>      response){
    auto left=request->left;
    auto right=request->right;
    sensor_msgs::msg::Image result_rgb, result_depth;

    //do procressing
    response->rgb = result_rgb;
    response->depth = result_depth;
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sent back processed rgbd data");
}
    
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("sensor_processing");
    
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sensor_processing node launched");
    //creation of RGBD data subscription and service
    rclcpp::Service<robo_messages::srv::Stereo2RGBD>::SharedPtr service_rgbd = 
        node->create_service<robo_messages::srv::Stereo2RGBD>("stereo2rgbd", &stereo2rgbd_service_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}




