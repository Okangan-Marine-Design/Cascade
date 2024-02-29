
#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

using namespace std::chrono_literals;
std::string filename = "image.png"; 

class ImagePublisherNode : public rclcpp::Node
{
public:
    ImagePublisherNode() : Node("image_publisher_node")
    { 
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/hardware/rawImage", 10);
        timer_ = this->create_wall_timer(
            500ms, std::bind(&ImagePublisherNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        auto message = sensor_msgs::msg::Image();
        // Assuming you have a method to fetch image data from the file
        fetchImageDataFromFile(filename, message);
        publisher_->publish(message);
    }

    // Method to fetch image data from a file and populate the Image message
    void fetchImageDataFromFile(const std::string& filename, sensor_msgs::msg::Image& imageMsg)
    {
        // Your implementation to read image data from file and populate imageMsg
        // Example:
        // Read image file and fill in imageMsg data, width, height, encoding, etc.
        // For simplicity, let's assume it's already implemented
        // Here you would read the image file and populate imageMsg data
        // For demonstration purposes, we'll just set some dummy data

        // Set width and height of the image
        imageMsg.width = 640;
        imageMsg.height = 480;

        // Set encoding (e.g., "rgb8", "bgr8", etc.)
        imageMsg.encoding = "rgb8";

        // Set some dummy image data (replace with actual image data)
        // For simplicity, we'll just set all pixel values to 0
        int dataSize = imageMsg.width * imageMsg.height * 3;
        imageMsg.data.resize(dataSize, 0);
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

