#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

using namespace std::chrono_literals;
std::string filename = "image.jpg"; 

class ImagePublisherNode : public rclcpp::Node
{
public:
    ImagePublisherNode() : Node("image_publisher_node")
    { 
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/sensors/camera/rgb", 10);
        timer_ = this->create_wall_timer(
            500ms, std::bind(&ImagePublisherNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        auto message = sensor_msgs::msg::Image();
        // Assuming you have a method to fetch image data from the file
        fetchImageDataFromFile(filename, message);
        //publisher_->publish(message);
    }

    // Method to fetch image data from a file and populate the Image message
    void fetchImageDataFromFile(const std::string& filename, sensor_msgs::msg::Image& imageMsg)
    {
        cv::Mat image = cv::imread(filename);
        
        if(!image.empty()) {
            cv_bridge::CvImage cvbImage=cv_bridge::CvImage(std_msgs::msg::Header(),std::string("rgb8"),image);
            cvbImage.toImageMsg(imageMsg);
        }
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImagePublisherNode>());
    rclcpp::shutdown();
    return 0;
}

