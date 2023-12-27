#include "rclcpp/rclcpp.hpp"
#include "System.h"

rclcpp::Client<package::srv::client_type>::SharedPtr imu_client;

void odom_loop(){//for clarity in main loop
    auto request = std::make_shared<package::srv::type::Request>();
    
    request->data=variable;//setting requesting data

    while (!client->wait_for_service(1s)) {
        if (!rclcpp::ok()) {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
            return 0;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not responding, trying again...");
    }

    while(rclcpp::ok()){
        auto result = client->async_send_request(request);//send request using client
        if (rclcpp::spin_until_future_complete(node, result) ==
            rclcpp::FutureReturnCode::SUCCESS){
            while(!result.valid()){result.wait();}//may not be required
            auto temp=*result.get();//must copy to temp variable because a future can only be accessed once
                                    
            std::cout<<"got service data: "<<temp.data<<'\n';
            //imu integration here
            //linear and rotational velocity reading * time elapsed 
            //-> estimation of current position and orientation
            //publishing location estimate after every update
            //
            //find specific imu & position data type and write dummy system
            //TODO: think about how depth information can be integrated
            //
            //smoothing/filter types to use for less noisy data? 
            //
            //look into how to write line based visual odometry if needed
        } 
        else {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service");
        }
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("odom");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created odometry node!");
        
    //~~publisher creation example~~
    //
    //TODO: add publisher code

    imu_client = node->create_client<package::srv::client_type>("imu");//use name of service being requested from

    odom_loop();

    rclcpp::shutdown();

    return 0;
}
