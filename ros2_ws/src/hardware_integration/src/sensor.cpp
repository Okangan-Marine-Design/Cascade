#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class Sensor : public rclcpp::Node
{
public:
    Sensor()
    : Node("sensor")
    {
        subscription_ =
        this->create_subscription<std_msgs::msg::String>("/hardware/rawJson", 10, this->json_callback);
        RCLCPP_INFO(this->get_logger(), "created sensor node");
    }

    void json_callback(std_msgs::msg::String::UniquePtr msg)
    {
        //parseReading(msg);
    }
    virtual void parseReading(std_msgs::msg::String::UniquePtr msg);

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Sensor>());
    rclcpp::shutdown();
    return 0;
}
