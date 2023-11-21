#include<iostream>
#include<algorithm>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "stereo-slam-node.hpp"
#include "System.h"

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        std::cerr << "\nUsage: ros2 run orbslam stereo path_to_vocabulary path_to_settings do_Rectify" << std::endl;
        return 1;
    }

    rclcpp::init(argc, argv);

    // malloc error using new.. try shared ptr
    // Create SLAM system. It initializes all system threads and gets ready to process frames.

    bool visualization = true;
    ORB_SLAM3::System SLAM(argv[1], argv[2], ORB_SLAM3::System::STEREO, visualization);
    auto node = std::make_shared<StereoSlamNode>(&SLAM, argv[2], argv[3]);
    std::cout << "============================ " << std::endl;

    rclcpp::Client<slam_messages::srv::Stereo>::SharedPtr client =
    node->create_client<slam_messages::srv::Stereo>("stereo");

    auto request = std::make_shared<slam_messages::srv::Stereo::Request>();
    while (!client->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
            return 0;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "slam_server not responding, trying again...");
    }
    while(rclcpp::ok()){
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
            temp.right.encoding = "rgb8";
            temp.left.encoding = "rgb8";
            node->GrabStereo(
                    std::make_shared<sensor_msgs::msg::Image>(temp.left),
                    std::make_shared<sensor_msgs::msg::Image>(temp.right));
        } 
        else {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service slam_server");
        }
    }

    rclcpp::shutdown();

    return 0;
}
