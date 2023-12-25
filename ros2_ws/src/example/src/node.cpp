/* example/node.cpp
 * written by Eryk Halicki
 * 
 * This file contains example c++ pseudo-code for ros2 node creation
 * contains examples for how to create and use:
 *  publisher
 *  subscriber
 *  service
 *  client
 *
 * Also contains example data logging using ROS_INFO
 * TODO: add other types of messages (more critical types)
 *
 * not meant to actually run, just for use as reference
 */

#include "rclcpp/rclcpp.hpp"
#include "System.h"

void callback(const package::msg::msg_type &msg){
    std::cout<<"got messgage from /topic";
}

void service_callback(const std::shared_ptr<package::srv::type::Request> request,
    std::shared_ptr<package::srv::type::Response> response){

    response->data = variable;
    std::cout<<"sending back data";
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("node");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created example node!");
        
    //~~subscription creation example~~
    //
    rclcpp::Subscription<package::msg::msg_type>::SharedPtr subscription=
    node->create_subscription<package::msg::msg_type>("/topic",10, &callback);
    
    //~~publisher creation example~~
    //
    //TODO: add publisher code

    //~~service creation example~~
    rclcpp::Service<package::srv::type>::SharedPtr service =
    node->create_service<package::srv::type>("service", &service_callback);

    //~~client creation example~~
    //
    rclcpp::Client<package::srv::client_type>::SharedPtr client =
    node->create_client<package::srv::client_type>("service");//use name of service being requested from


    //~~client request exmaple~~
    //
    auto request = std::make_shared<package::srv::type::Request>();
    
    request->data=variable;//setting requesting data

    while (!client->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
            return 0;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not responding, trying again...");
    }
    auto result = client->async_send_request(request);//send request using client
    // Wait for the result
    if (rclcpp::spin_until_future_complete(node, result) ==
        rclcpp::FutureReturnCode::SUCCESS){
        while(!result.valid()){result.wait();}//may not be required
        //TODO: test if this is still needed

        //README: async_send_request(...) doesnt return a shared_future 
        //it returns a future, which is why calling result.get() twice breaks the program
        //https://github.com/ros2/rclcpp/issues/1968
        //
        auto temp=*result.get();//must copy to temp variable because a future can only be accessed once
        std::cout<<"got service data: "<<temp.data<<'\n';
    } 
    else {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service");
    }

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
