#include "rclcpp/rclcpp.hpp"
#include "System.h"
#include <chrono>
#include <memory>
#include <string>

#include "std_msgs/msg/string.hpp"

class serialIntakeNode : public rclcpp::Node{
    private:
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
        size_t count_;
    public:
        serialIntakeNode(): Node("serialIntakeNode"), count_(0){
            publisher_ = this->create_publisher<std_msgs::msg::String>("/sensors/rawJson", 10);
            this->publisher_->publish(message);
        }
};

int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MinimalPublisher>());
    rclcpp::shutdown();
    return 0;
}
