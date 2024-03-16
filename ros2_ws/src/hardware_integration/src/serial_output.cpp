#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/motor_throttle.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class SerialOutputNode : public rclcpp::Node
{
public:
    SerialOutputNode() : Node("serial_output_node")
    { 
         subscription_ =
            this->create_subscription<cascade_msgs::msg::MotorThrottle>("/motor_throttle", 10, std::bind(&SerialOutputNode::throttle_callback, this, _1));
    }

private:
    void throttle_callback(cascade_msgs::msg::MotorThrottle msg)
    {
        //publish all motor outputs to serial (ESP32)
    }

    rclcpp::Subscription<cascade_msgs::msg::MotorThrottle>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SerialOutputNode>());
    rclcpp::shutdown();
    return 0;
}

