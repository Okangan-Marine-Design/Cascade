#include "rclcpp/rclcpp.hpp"
#include <octomap/math/Pose6D.h>
#include <octomap/math/Quaternion.h>
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
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Vector3.h>
#include <tf2/LinearMath/Quaternion.h>

#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

using namespace std;
using namespace octomap;
using namespace octomath;
using namespace cv_bridge;

OcTree tree (0.05);
pose6d current_pose = pose6d(0,0,0,0,0,0);
geometry_msgs::msg::PoseStamped lastPoseMsg;
unsigned int current_id=0;
bool inserted=false;
std::shared_ptr<rclcpp::Node> node;

float depth_to_meters(float d){
    return d;
}

Pointcloud rgbd2pointcloud(const sensor_msgs::msg::Image depth){
    Pointcloud result;
    //for: all points in img
    //  using camera specifications (FOV), create real world relative x and y coordinates for each pixel
    //  octomap::Pointcloud::push_back (float x,float y,float z)
    //TODO: google camera/img projection?
    //projection matrix?
    //void buildPointCloud(
    cv::Mat depth_img = cv_bridge::toCvCopy(depth)->image;
    int w = depth_img.cols;
    int h = depth_img.rows;
    //Camera1.fx: 617.201
    //Camera1.fy: 617.362
    //Camera1.cx: 324.637
    //Camera1.cy: 242.462
    double cx = 320;
    double cy = 240;
    double fx_inv = 1.0 / 554;
    double fy_inv = 1.0 / 554;
    float temp_x,temp_y,temp_z;
    for (int u = 0; u < w; ++u){
    auto rowstart = node->now();
    for (int v = 0; v < h; ++v){
        float z = depth_to_meters(depth_img.at<float>(v, u));   
        if (z > 0 && z < 10){  
            temp_x = z * ((u - cx) * fx_inv);
            temp_y = z * ((v - cy) * fy_inv);
            temp_z = z;  
            result.push_back(temp_x,temp_y,temp_z);
            }
        }  
        auto rowend = node->now();
        auto rowdiff = rowend - rowstart;
        //RCLCPP_INFO(node->get_logger(), "inserted column %.i in time(sec) : %.4f",u, rowdiff.seconds());
    }
    result.transform(current_pose);//changes relative pose to absolute pose
    return result;
}

void printEulerPose(const geometry_msgs::msg::PoseStamped& msgP) {
    geometry_msgs::msg::Quaternion msg=msgP.pose.orientation;
    // Placeholder implementations for getRoll, getPitch, and getYaw
    double roll = atan2(2.0 * (msg.w * msg.x + msg.y * msg.z),
                        1.0 - 2.0 * (msg.x * msg.x + msg.y * msg.y));
    double pitch = asin(2.0 * (msg.w * msg.y - msg.z * msg.x));
    double yaw = atan2(2.0 * (msg.w * msg.z + msg.x * msg.y),
                       1.0 - 2.0 * (msg.y * msg.y + msg.z * msg.z));

    // Print Euler angles
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Euler Pose: Roll: %.1f, Pitch: %.1f, Yaw: %.1f",roll,pitch,yaw);
}

void rgbd_subscription_callback(const robo_messages::msg::RGBD &msg){
    //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "got rgbd!");
    Pointcloud pc = rgbd2pointcloud(msg.depth);
    //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "converted to point cloud!");
    tree.insertPointCloud(pc,current_pose.trans());
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Tree Volume: %.4f",tree.volume());
}

void odometry_subscription_callback(const geometry_msgs::msg::PoseStamped &msg){
    current_pose=Pose6D(Vector3(
                                    msg.pose.position.x,
                                    msg.pose.position.y,
                                    msg.pose.position.z),
                        Quaternion(
                                    msg.pose.orientation.w,
                                    msg.pose.orientation.x,
                                    msg.pose.orientation.y,
                                    msg.pose.orientation.z)
                        );
    //printEulerPose(msg);
    lastPoseMsg=msg;
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
    node = rclcpp::Node::make_shared("map");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created map node!");
        
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr odometry_subscription=
    node->create_subscription<geometry_msgs::msg::PoseStamped>("/model/box/pose",10, &odometry_subscription_callback);

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
    tree.write(string("testOC.ot"));

    return 0;
}
