#include "rclcpp/rclcpp.hpp"
#include <opencv2/core/core.hpp>
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "cascade_msgs/msg/image_with_pose.hpp"
#include "cascade_msgs/srv/find_object.hpp"
#include <opencv2/core/core.hpp>
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Vector3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "bonxai/bonxai.hpp"

using namespace std;
using namespace cv_bridge;
using std::placeholders::_1;
using std::placeholders::_2;

const float MAX_DIST=15;
std::shared_ptr<rclcpp::Node> node;
bool inserting=false;

double voxel_resolution = 0.05;
Bonxai::VoxelGrid<float> grid( voxel_resolution );


float depth_to_meters(float d){
    return d;
}

void find_object_callback(const std::shared_ptr<cascade_msgs::srv::FindObject::Request> request,
                                        std::shared_ptr<cascade_msgs::srv::FindObject::Response>      response)
{
        
}

void rgbd2pointcloud(const cascade_msgs::msg::ImageWithPose img) {
    auto accessor = grid.createAccessor();
    if(inserting) return;
    inserting=true;//making sure we dont insert multiple depth scans at once
    cv::Mat depth_img = cv_bridge::toCvCopy(img.image)->image;
    int w = depth_img.cols;
    int h = depth_img.rows;
    double cx = 320; // TODO: Use parameters for camera projection info
    double cy = 240; // Current parameters are Gazebo Garden depth cam defaults
    double fx_inv = 1.0 / 554;
    double fy_inv = 1.0 / 554;

    // Convert current pose to tf2 Transform
    tf2::Transform tf_current_pose;
    tf2::fromMsg(img.pose, tf_current_pose);

    // Robot's rotation matrix
    tf2::Matrix3x3 tf_R(tf_current_pose.getRotation());

    for (int u = 0; u < w; ++u) {
        for (int v = 0; v < h; ++v) {
            float z = depth_to_meters(depth_img.at<float>(v, u));   
            if (z > 0 && z < MAX_DIST) {  
                float x = z * ((u - cx) * fx_inv); // Calculate real world projection of each pixel
                float y = z * ((v - cy) * fy_inv);

                // Apply rotation to the point
                tf2::Vector3 rotated_point = tf_R * tf2::Vector3(x, y, z);
                x = rotated_point.x();
                y = rotated_point.y();
                z = rotated_point.z();

                // Translate the point according to robot's pose
                x += img.pose.position.x;
                y += img.pose.position.y;
                z += img.pose.position.z;

                Bonxai::CoordT coord = grid.posToCoord(x, y, z);
                accessor.setValue(coord, 1.0f); // Set voxel value to 1.0
            }
        }  
    }
    inserting=false;
}

void img_subscription_callback(const cascade_msgs::msg::ImageWithPose &img_msg){
    //possibly add a queue for inserting the depth maps?
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    node = rclcpp::Node::make_shared("mapping_server");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created mapping_server node!");

    rclcpp::Subscription<cascade_msgs::msg::ImageWithPose>::SharedPtr img_subscription=
    node->create_subscription<cascade_msgs::msg::ImageWithPose>("/semantic_depth_with_pose",10, &img_subscription_callback);
    rclcpp::Service<cascade_msgs::srv::FindObject>::SharedPtr service=node->create_service<cascade_msgs::srv::FindObject>("find_object", &find_object_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
