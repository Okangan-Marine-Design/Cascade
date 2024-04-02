#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "rclcpp/rclcpp.hpp"
#include "cascade_msgs/msg/movement_command.hpp"
#include "cascade_msgs/msg/status.hpp"
#include "cascade_msgs/msg/sensor_reading.hpp"
#include "cascade_msgs/msg/goal_pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

class MotorCortexNode : public rclcpp::Node
{
    public:
        MotorCortexNode() 
        : Node("motor_cortex_node"){ 

            goal_pose_subscription_ = this->create_subscription<cascade_msgs::msg::GoalPose>("/current_goal_pose", 10, std::bind(&MotorCortexNode::goal_pose_callback, this, _1));
            current_pose_subscription_ = this->create_subscription<geometry_msgs::msg::PoseStamped>("/pose", 10, std::bind(&MotorCortexNode::current_pose_callback, this, _1));

            //adds all publishers to a map
            
            pidPublisherMap.insert(std::pair{"yaw", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/yaw/target", 10)});
            pidPublisherMap.insert(std::pair{"pitch", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/pitch/target", 10)});
            pidPublisherMap.insert(std::pair{"roll", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/roll/target", 10)});
            pidPublisherMap.insert(std::pair{"surge", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/surge/target", 10)});
            pidPublisherMap.insert(std::pair{"sway", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/sway/target", 10)});
            pidPublisherMap.insert(std::pair{"heave", this->create_publisher<cascade_msgs::msg::SensorReading>("/PID/heave/target", 10)});
            status_publisher_ = this->create_publisher<cascade_msgs::msg::Status>("/current_goal_status", 10);
            timer_ = this->create_wall_timer(
                50ms, std::bind(&MotorCortexNode::updateSetPoints, this));

        }
    private:
        void goal_pose_callback(cascade_msgs::msg::GoalPose msg){
            currentGoalPoseMsg=msg;
        }

        void current_pose_callback(geometry_msgs::msg::PoseStamped msg){
            currentPoseMsg=msg;
        }
        float calculateTimeFromSpeed(){
            //implement
            //how fast do we want to be going based on distance from goal
            //if 5m x and 1m y and max speed is 2.5m/s
            //2s (because x is the larger value, 5/2.5)
            //then y speed will be 1/2 = 0.5m/s
            return 1;
        }
        geometry_msgs::msg::Vector3 calculateRelativeTranslation(const geometry_msgs::msg::Pose& current_pose,
                                                         const geometry_msgs::msg::Pose& goal_pose)
        {
            // Convert poses to tf2::Transform
            tf2::Transform tf_current, tf_goal;
            tf2::fromMsg(current_pose, tf_current);
            tf2::fromMsg(goal_pose, tf_goal);
        
            // Calculate the inverse of the current pose
            tf2::Transform tf_current_inv = tf_current.inverse();
        
            // Transform the goal pose from the current frame to the robot's frame
            tf2::Transform tf_goal_relative = tf_current_inv * tf_goal;
        
            // Calculate the relative translation
            tf2::Vector3 translation = tf_goal_relative.getOrigin();
        
            // Convert the result back to geometry_msgs::msg::Vector3
            geometry_msgs::msg::Vector3 relative_translation;
            relative_translation.x = translation.x();
            relative_translation.y = translation.y();
            relative_translation.z = translation.z();
        
            return relative_translation;
        }
        
        void updateSetPoints(){
            //gets called on a regular interval, calculates difference between current pose and goal pose, then publishes neccessary PID setpoints 
            cascade_msgs::msg::SensorReading 
                pitchMsg=cascade_msgs::msg::SensorReading(),
                yawMsg=cascade_msgs::msg::SensorReading(),
                rollMsg=cascade_msgs::msg::SensorReading(),
                surgeMsg=cascade_msgs::msg::SensorReading(),
                heaveMsg=cascade_msgs::msg::SensorReading(),
                swayMsg=cascade_msgs::msg::SensorReading();

            //calculating required rotation 
            float yawDiff=atan2(currentGoalPoseMsg.pose.position.x-currentPoseMsg.pose.position.x,
                        currentGoalPoseMsg.pose.position.y-currentPoseMsg.pose.position.y)*(180/3.1415926);
            
            float pitchDiff=atan2(currentGoalPoseMsg.pose.position.z-currentPoseMsg.pose.position.z,
                        currentGoalPoseMsg.pose.position.y-currentPoseMsg.pose.position.y)*(180/3.1415926);
            pitchDiff=0;
            yawDiff=0;//dont try to orient to the goal pose, just do translational movements

            geometry_msgs::msg::Vector3 relative_translation = calculateRelativeTranslation(currentPoseMsg.pose,currentGoalPoseMsg.pose);

            pitchMsg.data=pitchDiff;
            yawMsg.data=yawDiff;
            rollMsg.data=0;
            surgeMsg.data=relative_translation.x;
            heaveMsg.data=relative_translation.z;
            swayMsg.data=relative_translation.y;
            pitchMsg.header.stamp=this->now();
            yawMsg.header.stamp=this->now();
            rollMsg.header.stamp=this->now();
            surgeMsg.header.stamp=this->now();
            swayMsg.header.stamp=this->now();
            heaveMsg.header.stamp=this->now();

            pidPublisherMap["pitch"]->publish(pitchMsg);
            pidPublisherMap["yaw"]->publish(yawMsg);
            pidPublisherMap["roll"]->publish(rollMsg);
            pidPublisherMap["surge"]->publish(surgeMsg);
            pidPublisherMap["sway"]->publish(swayMsg);
            pidPublisherMap["heave"]->publish(heaveMsg);


            //how will roll pid be handled? always 0? probably for now yes
            //now figure out how to tranform the pose

            auto message = cascade_msgs::msg::Status();
            //what will status messages mean?
            //what info do we need to pass to the motion planner node?
            //0=moving, 1=reached destination, -1=failed?
            message.status = 0;
            status_publisher_->publish(message);
        }
        cascade_msgs::msg::GoalPose currentGoalPoseMsg; 
        geometry_msgs::msg::PoseStamped currentPoseMsg;
        rclcpp::Subscription<cascade_msgs::msg::GoalPose>::SharedPtr  goal_pose_subscription_;
        rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr  current_pose_subscription_;
        rclcpp::Publisher<cascade_msgs::msg::Status>::SharedPtr status_publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        std::map<std::string, rclcpp::Publisher<cascade_msgs::msg::SensorReading>::SharedPtr> pidPublisherMap;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotorCortexNode>());
  rclcpp::shutdown();
  return 0;
}
