#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/sensor_reading.hpp"
#include "geometry_msgs/msg/pose.hpp"

using std::placeholders::_1;

class MotorCortexNode : public rclcpp::Node
{
    public:
        MotorCortexNode() 
        : Node("motor_cortex_node"){ 

            pose_subscription_ = this->create_subscription<geometry_msgs::msg::Pose>("/current_goal_pose", 10, std::bind(&MotorCortexNode::goal_pose_callback, this, _1));

            status_publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/current_goal_status", 10);

            //adds all publishers to a map
            
            pidPublisherMap.insert(std::pair{"yaw", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/yaw/target", 10)});
            pidPublisherMap.insert(std::pair{"pitch", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/pitch/target", 10)});
            pidPublisherMap.insert(std::pair{"roll", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/roll/target", 10)});
            pidPublisherMap.insert(std::pair{"surge", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/surge/target", 10)});
            pidPublisherMap.insert(std::pair{"sway", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/sway/target", 10)});
            pidPublisherMap.insert(std::pair{"heave", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/heave/target", 10)});

        }
    private:
        void goal_pose_callback(geometry_msgs::msg::Pose msg){
            auto message = cascade_msgs::msg::Status();
            message.status = 0;
            status_publisher_->publish(message);
        }

        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr pose_subscription_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher_;
        std::map<std::string, rclcpp::Publisher<cascade_msgs::msg::SensorReading>::SharedPtr> pidPublisherMap;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotorCortexNode>());
  rclcpp::shutdown();
  return 0;
}
