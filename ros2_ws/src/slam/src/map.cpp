#include "rclcpp/rclcpp.hpp"
#include "System.h"

void odometry_subscription_callback(const robo_messages::msg::pos_estimate &msg){
    std::cout<<"got messgage from /topic";
    //integrate into map
    //update position of sub agent in 3d map space
}

void objects_subscription_callback(const robo_messages::msg::object_location_estimates &msg){
    std::cout<<"got messgage from /topic";
    //integrate into map
    //update probability distribution of object based on reading 
    //and on sub location 
}

void world_state_service_callback(const std::shared_ptr<package::srv::type::Request> request,
    std::shared_ptr<package::srv::type::Response> response){

    response->data = variable;
    std::cout<<"sending back data";
    //send back list of all world states
}

void map_object_service_callback(const std::shared_ptr<package::srv::type::Request> request,
    std::shared_ptr<package::srv::type::Response> response){

    response->data = variable;
    std::cout<<"sending back data";
    //specific objects will be sent back by type requested?
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    //creates node as shared pointer
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("map");
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
            "created map node!");
        
    rclcpp::Subscription<package::msg::msg_type>::SharedPtr subscription=
    node->create_subscription<package::msg::msg_type>("/odom",10, &odometry_subscription_callback);

    rclcpp::Subscription<package::msg::msg_type>::SharedPtr subscription=
    node->create_subscription<package::msg::msg_type>("/objects",10, &objects_subscription_callback);

    
    rclcpp::Service<package::srv::type>::SharedPtr service =
    node->create_service<package::srv::type>("world_states", &world_state_service_callback);
    
    rclcpp::Service<package::srv::type>::SharedPtr service =
    node->create_service<package::srv::type>("map_objects", &map_object_service_callback);

    //simple, all work is done within the callbacks

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
