#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/srv/find_object.hpp"
#include <boost/interprocess/shared_memory_object.hpp>

class ObjectLocatorNode : public rclcpp::Node
{
    public:
        ObjectLocatorNode() : Node("object_locator_node")
        { 
            service=this->create_service<cascade_msgs::srv::FindObject>("object_locator", &ObjectLocatorNode::find_object_callback);
        }
    private:
        void find_object_callback(const std::shared_ptr<cascade_msgs::srv::FindObject::Request> request,
                                        std::shared_ptr<cascade_msgs::srv::FindObject::Response>      response)
        {
        
        }
        rclcpp::Service<cascade_msgs::srv::FindObject>::SharedPtr service;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ObjectLocatorNode>());
  rclcpp::shutdown();
  return 0;
}
