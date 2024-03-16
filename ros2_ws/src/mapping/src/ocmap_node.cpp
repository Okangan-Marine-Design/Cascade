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

Pointcloud rgbd2pointcloud(const sensor_msgs::msg::Image depth)
        {
            //function takes in rgb-d image and returns a pointcloud object to be inserted into the octomap
            Pointcloud result;
            cv::Mat depth_img = cv_bridge::toCvCopy(depth)->image;
            int w = depth_img.cols;
            int h = depth_img.rows;
            double cx = 320;//TODO: create node parameters for camera projection info
            double cy = 240;//current parameters are Gazebo Garden depth cam defaults
            double fx_inv = 1.0 / 554;
            double fy_inv = 1.0 / 554;
            float x,y;
            for (int u = 0; u < w; ++u){
                for (int v = 0; v < h; ++v){
                    float z = depth_to_meters(depth_img.at<float>(v, u));   
                    if (z > 0 && z < MAX_DIST){  
                        x = z * ((u - cx) * fx_inv);//calculating the real world projection of each pixel
                        y = z * ((v - cy) * fy_inv);
                        result.push_back(x,y,z);//inserting point into the pointcloud
                    }
                }  
            }
            result.transform(current_pose);//changes pointcloud from camera frame to map frame
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
