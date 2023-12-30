#include "rclcpp/rclcpp.hpp"
#include "robo_messages/srv/rgbd.hpp"
#include "robo_messages/srv/stereo.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include <memory>
#include <string.h>
#include "main.hpp"

sensor_msgs::msg::Image recent_front_right_cam;
sensor_msgs::msg::Image recent_front_left_cam;
sensor_msgs::msg::Image recent_front_cam_rgb;
sensor_msgs::msg::Image recent_front_cam_depth;
sensor_msgs::msg::Imu recent_imu;

rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr front_cam_rgb_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr front_cam_depth_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr front_right_cam_sub;
rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr front_left_cam_sub;
//rclcpp::Subscription<float>::SharedPtr depth; 
//how to subscribe to float type topic?

//rclcpp::Subscription<???>::SharedPtr hydrophone; 
//TODO: figure out what kind of data we get from hydrophones

void front_rgb_subscription_callback(const sensor_msgs::msg::Image &msg){
    recent_front_cam_rgb=msg;
}
void front_depth_subscription_callback(const sensor_msgs::msg::Image &msg){
    recent_front_cam_depth=msg;
}
void front_right_cam_subscription_callback(const sensor_msgs::msg::Image &msg){
    recent_front_right_cam=msg;
}
void front_left_cam_subscription_callback(const sensor_msgs::msg::Image &msg){
    recent_front_left_cam=msg;
    //TODO:add depth processing on every left or right callback, doesnt really matter
    //add some kind of timestamp for more accurate stereo photo matching? does it even matter?
}
void bottom_cam_subscription_callback(const sensor_msgs::msg::Image &msg){
    recent_bottom_cam=msg;
}
void imu_subscription_callback(const sensor_msgs::msg::Imu &msg){
    recent_imu=msg;
    //TODO: research smoothing/filtering for imu data
    //will this even be an issue with underwater movement? will there be significant data noise coming from motors?
}

void front_rgbd_service_callback(const std::shared_ptr<robo_messages::srv::RGBD::Request> request,
    std::shared_ptr<robo_messages::srv::RGBD::Response>      response){
    response->rgb = recent_front_cam_rgb;
    response->depth = recent_front_cam_depth;
}
void front_stereo_service_callback(const std::shared_ptr<robo_messages::srv::Stereo::Request> request,
    std::shared_ptr<robo_messages::srv::Stereo::Response>      response){
    response->left = recent_front_left_cam;
    response->right = recent_front_right_cam;
}
void bottom_cam_service_callback(const std::shared_ptr<robo_messages::srv::Stereo::Request> request,
    std::shared_ptr<robo_messages::srv::Stereo::Response>      response){
    response->data = recent_bottom_cam;
    //this will use primitive ROS2 img type since its a mono camera for sure
}
void imu_service_callback(const std::shared_ptr<robo_messages::srv::Imu::Request> request,
    std::shared_ptr<robo_messages::srv::Imu::Response>      response){
    response->data = recent_imu;
}

int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("sensor_server");
    
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sensor_server launched!");
    //creation of front facing RGBD data subscription and service
    //subscriptions used only in simulator, no real camera can reliably provide direct rbgd data underwater because of IF light attenuation 
    rclcpp::Service<slam_messages::srv::RGBD>::SharedPtr front_rgbd_service = 
        node->create_service<slam_messages::srv::RGBD>("front_rgbd", &rgbd_service_callback);
    front_cam_rgb_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/front/rgb",10, &front_rgb_subscription_callback);
    front_cam_depth_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/front/depth",10,&front_depth_subscription_callback);

    //image services and subscriptions
    rclcpp::Service<sub_messages::srv::Stereo>::SharedPtr front_stereo_service = 
        node->create_service<robo_messages::srv::Stereo>("front_stereo", &front_stereo_service_callback);
    rclcpp::Service<sub_messages::srv::Stereo>::SharedPtr bottom_cam_service = 
        node->create_service<robo_messages::srv::Stereo>("bottom_cam", &bottom_cam_service_callback);

    front_right_cam_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/front/right",10, &front_right_cam_subscription_callback);
    front_left_cam_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/front/left",10,&front_left_cam_subscription_callback);
    bottom_cam_sub = node->create_subscription<sensor_msgs::msg::Image>("/camera/bottom",10,&bottom_cam_subscription_callback);

    //imu subscription and service
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub =
        node->create_subscription<sensor_msgs::msg::Imu>("/imu",100, &imu_subscription_callback);
    rclcpp::Service<sub_messages::srv::Stereo>::SharedPtr imu_service = 
        node->create_service<robo_messages::srv::Stereo>("imu", &imu_service_callback);

    //TODO: add hydrophone subscription
    //TODO: add hydrophone service?
    //TODO: add depth sensor subscription
    //TODO: add depth sensor service

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "All subscriptions and services started. Ready to send sensor data.");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
