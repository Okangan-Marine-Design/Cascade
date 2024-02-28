#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
std::string filename = "sensor.json"; 

class SerialIntakeNode : public rclcpp::Node
{
    public:
        SerialIntakeNode() : Node("serial_intake_node")
        { 
            publisher_ = this->create_publisher<std_msgs::msg::String>("/hardware/rawJson", 10);
            timer_ = this->create_wall_timer(
                500ms, std::bind(&SerialIntakeNode::timer_callback, this));
        }
    private:
        std::string fetchJsonStringFromFile(const std::string& filename) 
        {//method for testing with json files
            std::ifstream file(filename);
            std::string json_string;

            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    json_string += line;
                }
                file.close();
            } else {
                std::cerr << "Unable to open file: " << filename << std::endl;
            }
            return json_string;
        }

        void timer_callback()
        {
            auto message = std_msgs::msg::String();
            message.data = fetchJsonStringFromFile(filename);
            publisher_->publish(message);
        }
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SerialIntakeNode>());
  rclcpp::shutdown();
  return 0;
}
