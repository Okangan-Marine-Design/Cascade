#include "rclcpp/rclcpp.hpp"
#include <octomap/math/Pose6D.h>
#include <octomap/math/Quaternion.h>
#include <octomap/octomap.h>
#include <opencv2/core/core.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "cascade_msgs/msg/image_with_pose.hpp"
#include <opencv2/core/core.hpp>
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Vector3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

using namespace std;
using namespace octomap;
using namespace octomath;
using namespace cv_bridge;
using std::placeholders::_1;
using std::placeholders::_2;

OcTree tree (0.05);
pose6d current_pose = pose6d(0,0,0,0,0,0);
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

void img_subscription_callback(const cascade_msgs::msg::ImageWithPose &img_msg){
    current_pose=Pose6D(Vector3(
                                    img_msg.pose.position.x,
                                    img_msg.pose.position.y,
                                    img_msg.pose.position.z),
                        Quaternion(
                                    img_msg.pose.orientation.w,
                                    img_msg.pose.orientation.x,
                                    img_msg.pose.orientation.y,
                                    img_msg.pose.orientation.z)
                        );
    Pointcloud pc = rgbd2pointcloud(img_msg.image);
    tree.insertPointCloud(pc,current_pose.trans());
    //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Tree Volume: %.4f",tree.volume());
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    node = rclcpp::Node::make_shared("octomap_server");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created octomap_server node!");
        
    rclcpp::Subscription<cascade_msgs::msg::ImageWithPose>::SharedPtr img_subscription=
    node->create_subscription<cascade_msgs::msg::ImageWithPose>("/semantic_depth_with_pose",10, &img_subscription_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();
    tree.write(string("test.ot"));

    return 0;
}
//TODO: convert whole file to object later
