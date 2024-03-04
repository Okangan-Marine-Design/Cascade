#include "rclcpp/rclcpp.hpp"
#include <octomap/math/Pose6D.h>
#include <octomap/math/Quaternion.h>
#include <octomap/octomap.h>
#include <opencv2/core/core.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include <opencv2/core/core.hpp>
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Vector3.h>
#include <tf2/LinearMath/Quaternion.h>
#include "message_filters/subscriber.h"
#include "message_filters/time_synchronizer.h"
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

void depth_subscription_callback(const sensor_msgs::msg::Image &msg){
    Pointcloud pc = rgbd2pointcloud(msg.depth);
    tree.insertPointCloud(pc,current_pose.trans());
    //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Tree Volume: %.4f",tree.volume());
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

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    node = rclcpp::Node::make_shared("map");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created octomap node!");
        
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr odometry_subscription=
    node->create_subscription<geometry_msgs::msg::PoseStamped>("/pose",10, &odometry_subscription_callback);

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr rgbd_subscription=
    node->create_subscription<sensor_msgs::msg::Image>("/semantic_depth",10, &depth_subscription_callback);
    
    rclcpp::spin(node);
    rclcpp::shutdown();
    tree.write(string("test.ot"));

    return 0;
}
