#include "rclcpp/rclcpp.hpp"
#include "System.h"

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    std::cout<<"hello world";
    rclcpp::spin();
    rclcpp::shutdown();

    return 0;
}
