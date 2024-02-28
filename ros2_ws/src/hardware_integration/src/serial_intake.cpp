#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class SerialIntakeNode : public rclcpp::Node
{
    public:
        SerialIntakeNode() : Node("serial_intake_node")
        { 
            publisher_ = this->create_publisher<std_msgs::msg::String>("/hardware/rawJson", 10);
        }
    private:
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SerialIntakeNode>());
  rclcpp::shutdown();
  return 0;
}
