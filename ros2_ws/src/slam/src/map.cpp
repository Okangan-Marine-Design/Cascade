#include "rclcpp/rclcpp.hpp"
#include "System.h"
#include <octomap>
#include <opencv2/core/core.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.h>

using namespace std;
using namespace octomap;

OcTree tree (0.05);
pose6d current_pose;
unsigned int current_id=0;

Pointcloud rgbd2pointcloud(const ImageMsg::SharedPtr depth){
    Pointcloud result;
    //for: all points in img
    //  using camera specifications (FOV), create real world relative x and y coordinates for each pixel
    //  octomap::Pointcloud::push_back (float x,float y,float z)
    //TODO: google camera/img projection?
    //projection matrix?
    result.transform(current_pose);//changes relative pose to absolute pose
    return result;
}

void rgbd_subscription_callback(const robo_messages::msg::RGBD &msg){
    ScanNode scan(rgbd2pointcloud(msg.depth),current_pose,current_id++);
    map.insertPointCloud(scan);
}

void odometry_subscription_callback(const robo_messages::msg::pose6d &msg){
    std::cout<<"got messgage from /odometry";
    current_pose=msg.data;
    //update absolute pose or sum up pose here?
    //TODO: create pose message type/ find interal pose6d type
}

void objects_subscription_callback(const robo_messages::msg::object_location_estimates &msg){
    std::cout<<"got messgage from /topic";
    //integrate into map
    //update probability distribution of object based on reading 
    //and on sub location 
}

void world_state_service_callback(const std::shared_ptr<package::srv::type::Request> request,
    std::shared_ptr<package::srv::type::Response> response){

    response->data = variable;
    std::cout<<"sending back data";
    //send back list of all world states
}

void map_object_service_callback(const std::shared_ptr<package::srv::type::Request> request,
    std::shared_ptr<package::srv::type::Response> response){

    response->data = variable;
    std::cout<<"sending back data";
    //location of specific objects will be sent back by type requested?
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("map");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created map node!");
        
    rclcpp::Subscription<package::msg::msg_type>::SharedPtr odometry_subscription=
    node->create_subscription<package::msg::msg_type>("/odom",10, &odometry_subscription_callback);

    rclcpp::Subscription<package::msg::msg_type>::SharedPtr rgbd_subscription=
    node->create_subscription<package::msg::msg_type>("/front_rgbd",10, &rgbd_subscription_callback);
    
    rclcpp::Subscription<package::msg::msg_type>::SharedPtr objects_subscription=
    node->create_subscription<package::msg::msg_type>("/objects",10, &objects_subscription_callback);


    rclcpp::Service<package::srv::type>::SharedPtr world_state_service =
    node->create_service<package::srv::type>("world_states", &world_state_service_callback);
    
    rclcpp::Service<package::srv::type>::SharedPtr map_object_service =
    node->create_service<package::srv::type>("map_objects", &map_object_service_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
