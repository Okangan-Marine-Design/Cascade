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
#include "bonxai/serialization.hpp"
#include <sstream>

using namespace std;
using namespace cv_bridge;
using std::placeholders::_1;
using std::placeholders::_2;

std::shared_ptr<rclcpp::Node> node;

double voxel_resolution = 0.2;
Bonxai::VoxelGrid<float> grid( voxel_resolution );

void updateGridFromMsg(const cascade_msgs::msg::VoxelGrid &msg){
    std::string serialized_data(msg.data.begin(), msg.data.end());

    std::istringstream ifile(serialized_data, std::ios::binary);
    
    char header[256];
    ifile.getline(header, 256);
    Bonxai::HeaderInfo info = Bonxai::GetHeaderInfo(header);
    auto g = Bonxai::Deserialize<float>(ifile, info);
    grid=std::move(g);
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    node = rclcpp::Node::make_shared("cost_map_server");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created cost_map_server node!");

    rclcpp::Subscription<cascade_msgs::msg::ImageWithPose>::SharedPtr img_subscription=
    node->create_subscription<cascade_msgs::msg::ImageWithPose>("/",10, &img_subscription_callback);
    rclcpp::Service<cascade_msgs::srv::FindObject>::SharedPtr service=node->create_service<cascade_msgs::srv::FindObject>("find_object", &find_object_callback);

    rclcpp::spin(node);
    rclcpp::shutdown();

    std::ofstream outputFile("map.bx", std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing" << std::endl;
        return 1;
    }

    Bonxai::Serialize(outputFile, grid);
    outputFile.close();
    return 0;
}
