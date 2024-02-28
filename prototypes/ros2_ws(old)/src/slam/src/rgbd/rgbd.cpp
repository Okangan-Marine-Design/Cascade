#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "rgbd-slam-node.hpp"
#include "System.h"

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    // malloc error using new.. try shared ptr
    // Create SLAM system. It initializes all system threads and gets ready to process frames.

    bool visualization = true;
    ORB_SLAM3::System SLAM("ORBvoc.txt", "gazebo_rgbd.yaml", ORB_SLAM3::System::RGBD, visualization);

    auto node = std::make_shared<RgbdSlamNode>(&SLAM);
    std::cout << "============================ " << std::endl;

    rclcpp::Client<robo_messages::srv::RGBD>::SharedPtr client =
    node->create_client<robo_messages::srv::RGBD>("front_rgbd");

    auto request = std::make_shared<robo_messages::srv::RGBD::Request>();
    while (!client->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
            return 0;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), strcat(const_cast<char*> (client->get_service_name())," not responding, trying again..."));
    }
    while(rclcpp::ok()){
        //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending request now!");
        auto result = client->async_send_request(request);
        // Wait for the result.
        if (rclcpp::spin_until_future_complete(node, result) ==
            rclcpp::FutureReturnCode::SUCCESS){
            while(!result.valid()){result.wait();}
            //std::cout<<"about to call grab!\n";
            //gets to this point, result.get() must be not working?
            //maybe find a way to get the result, then seperate it into parts (rgb and depth)?
            //
            //README: Turns out that async_send_request(...) doesnt return a shared_future 
            //it returns a future, which is why calling result.get() twice broke the program
            //https://github.com/ros2/rclcpp/issues/1968
            //
            auto temp=*result.get();
            temp.rgb.encoding = "rgb8";
            temp.depth.encoding = "mono8";
            node->GrabRGBD(
                    std::make_shared<sensor_msgs::msg::Image>(temp.rgb),
                    std::make_shared<sensor_msgs::msg::Image>(temp.depth));
        } 
        else {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service rgbd_server");
        }
    }

    rclcpp::shutdown();

    return 0;
}
