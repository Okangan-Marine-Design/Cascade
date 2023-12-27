#include "rclcpp/rclcpp.hpp"
#include "slam_messages/srv/rgbd.hpp"
#include "slam_messages/srv/stereo.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <memory>
#include <string.h>
#include "main.hpp"

//TODO: add the rest of the services and subscriptions
sensor_msgs::msg::Image recent_right;
sensor_msgs::msg::Image recent_left;
sensor_msgs::msg::Image recent_rgb;
sensor_msgs::msg::Image recent_depth;
sensor_msgs::msg::Imu recent_imu;

rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr rgb_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr right_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr left_sub;

void rgb_callback(const sensor_msgs::msg::Image &msg){
    recent_rgb=msg;
}
void depth_callback(const sensor_msgs::msg::Image &msg){
    recent_depth=msg;
}

void right_callback(const sensor_msgs::msg::Image &msg){
    recent_right=msg;
}
void left_callback(const sensor_msgs::msg::Image &msg){
    recent_left=msg;
    //TODO:add depth processing on every left or right callback, doesnt really matter
    //add some kind of timestamp for more accurate photo matching? does it even matter?
}

void imu_callback(const sensor_msgs::msg::Imu &msg){
    recent_imu=msg;
    //TODO: research smoothing for imu data
}

void send_rgbd(const std::shared_ptr<robo_messages::srv::RGBD::Request> request,
    std::shared_ptr<robo_messages::srv::RGBD::Response>      response){
    response->rgb = recent_rgb;
    response->depth = recent_depth;
}
void send_stereo(const std::shared_ptr<robo_messages::srv::Stereo::Request> request,
    std::shared_ptr<robo_messages::srv::Stereo::Response>      response){
    response->left = recent_left;
    response->right = recent_right;
}
    
void send_imu(const std::shared_ptr<robo_messages::srv::Imu::Request> request,
    std::shared_ptr<robo_messages::srv::Imu::Response>      response){
    response->data = recent_imu;
}

int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("sensor_server");
    
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sensor_server launched");
    //creation of RGBD data subscription and service
    rclcpp::Service<slam_messages::srv::RGBD>::SharedPtr service_rgbd = 
        node->create_service<slam_messages::srv::RGBD>("rgbd", &send_rgbd);
    rgb_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/rgb",10, &rgb_callback);
    depth_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/depth",10,&depth_callback);

    //stereo image service and subscription
    rclcpp::Service<sub_messages::srv::Stereo>::SharedPtr service_stereo = 
        node->create_service<robo_messages::srv::Stereo>("stereo", &send_stereo);
    right_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/right",10, &right_callback);
    left_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/left",10,&left_callback);

    //imu subscription
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub =
        node->create_subscription<sensor_msgs::msg::Imu>("/imu",100, &imu_callback);
    rclcpp::Service<sub_messages::srv::Stereo>::SharedPtr service_stereo = 
        node->create_service<robo_messages::srv::Stereo>("imu", &send_imu);


    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to send sensor data.");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
