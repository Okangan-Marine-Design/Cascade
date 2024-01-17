#include "rclcpp/rclcpp.hpp"
#include <octomap/math/Pose6D.h>
#include <octomap/octomap.h>
#include <opencv2/core/core.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "robo_messages/msg/rgbd.hpp"
#include "robo_messages/srv/object.hpp"
#include "robo_messages/srv/world_states.hpp"
#include "robo_messages/msg/object.hpp"
#include <opencv2/core/core.hpp>

using namespace std;
using namespace octomap;

OcTree tree (0.05);
pose6d current_pose;
unsigned int current_id=0;


Pointcloud rgbd2pointcloud(const sensor_msgs::msg::Image depth){
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
    Pointcloud pc = rgbd2pointcloud(msg.depth);
    ScanNode scan(&pc,current_pose,current_id++);
    tree.insertPointCloud(scan);
}

void odometry_subscription_callback(const geometry_msgs::msg::PoseStamped &msg){
    std::cout<<"got messgage from /odometry";
    //current_pose=msg.data;
    //update absolute pose or sum up pose here?
    //TODO: create pose message type/ find interal pose6d type
}

void objects_subscription_callback(const robo_messages::msg::Object &msg){
    std::cout<<"got messgage from /topic";
    //integrate into map
    //update probability distribution of object based on reading 
    //and on sub location 
}

void world_state_service_callback(const std::shared_ptr<robo_messages::srv::WorldStates::Request> request,
    std::shared_ptr<robo_messages::srv::WorldStates::Response> response){

    std::cout<<"sending back data";
    //send back list of all world states
}

void map_object_service_callback(const std::shared_ptr<robo_messages::srv::Object::Request> request,
    std::shared_ptr<robo_messages::srv::Object::Response> response){

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
        
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr odometry_subscription=
    node->create_subscription<geometry_msgs::msg::PoseStamped>("/pose",10, &odometry_subscription_callback);

    rclcpp::Subscription<robo_messages::msg::RGBD>::SharedPtr rgbd_subscription=
    node->create_subscription<robo_messages::msg::RGBD>("/front_rgbd",10, &rgbd_subscription_callback);
    
    rclcpp::Subscription<robo_messages::msg::Object>::SharedPtr objects_subscription=
    node->create_subscription<robo_messages::msg::Object>("/objects",10, &objects_subscription_callback);


    rclcpp::Service<robo_messages::srv::WorldStates>::SharedPtr world_state_service =
    node->create_service<robo_messages::srv::WorldStates>("world_states", &world_state_service_callback);
    
    rclcpp::Service<robo_messages::srv::Object>::SharedPtr map_object_service =
    node->create_service<robo_messages::srv::Object>("map_objects", &map_object_service_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
