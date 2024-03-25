#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/json_stamped.hpp"

using namespace std::chrono_literals;
std::string filename = "sensor.json"; 

class SerialIntakeNode : public rclcpp::Node
{
    public:
        SerialIntakeNode() : Node("serial_intake_node")
        { 
            publisher_ = this->create_publisher<cascade_msgs::msg::JsonStamped>("/hardware/rawJson", 10);
            timer_ = this->create_wall_timer(
                50ms, std::bind(&SerialIntakeNode::timer_callback, this));
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
            auto message = cascade_msgs::msg::JsonStamped();
            message.data = fetchJsonStringFromFile(filename);
            message.header.frame_id="map";
            message.header.stamp= this->now();
            publisher_->publish(message);
        }
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<cascade_msgs::msg::JsonStamped>::SharedPtr publisher_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SerialIntakeNode>());
  rclcpp::shutdown();
  return 0;
}
