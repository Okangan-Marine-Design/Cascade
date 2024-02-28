#include "rgbd-slam-node.hpp"

#include <opencv2/core/core.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
using std::placeholders::_1;

RgbdSlamNode::RgbdSlamNode(ORB_SLAM3::System* pSLAM)
:   Node("ORB_SLAM3_ROS2"){
    m_SLAM = pSLAM;
    pose_pub = this->create_publisher<geometry_msgs::msg::PoseStamped>("/pose", 10);
}

RgbdSlamNode::~RgbdSlamNode(){
    // Stop all threads
    m_SLAM->Shutdown();

    // Save camera trajectory
    m_SLAM->SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");
}
cv::Mat SE3ToCvMat(const Sophus::SE3<float>& se3) {
  // get 4x4 homogenous matrix
  const Eigen::Matrix<float, 4, 4> m = se3.matrix();
  // convert to cv::Mat
  cv::Mat cvMat(m.rows(), m.cols(), CV_32F);
  for (int i = 0; i < m.rows(); ++i) {
    for (int j = 0; j < m.cols(); ++j) {
      cvMat.at<float>(i, j) = m(i, j);
    }
  }
  return cvMat;
}
void RgbdSlamNode::GrabRGBD(const ImageMsg::SharedPtr msgRGB, const ImageMsg::SharedPtr msgD)
{
    // Copy the ros rgb image message to cv::Mat.
    try{
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB);
    }
    catch (cv_bridge::Exception& e){
        RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        return;
    }

    // Copy the ros depth image message to cv::Mat.
    try{
        cv_ptrD = cv_bridge::toCvShare(msgD);
    }
    catch (cv_bridge::Exception& e){
        RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        return;
    }
    //display the rbg image component
    cv::cvtColor(cv_ptrRGB->image,cv_ptrRGB->image, CV_BGR2RGB);
    cv::imshow("rgb",cv_ptrRGB->image);
    
    Sophus::SE3<float> raw_sophus_pose = m_SLAM->TrackRGBD(cv_ptrRGB->image, cv_ptrD->image, Utility::StampToSec(msgRGB->header.stamp));
    cv::Mat Tcw = SE3ToCvMat(raw_sophus_pose);//converting to cv mat
    geometry_msgs::msg::PoseStamped pose;
    pose.header.stamp = this->get_clock()->now();
    pose.header.frame_id ="map";

    cv::Mat rot = Tcw.rowRange(0,3).colRange(0,3); // Rotation information
    vector<float> q = ORB_SLAM3::Converter::toQuaternion(rot);
    cv::Mat twc = -rot.t()*Tcw.rowRange(0,3).col(3);

    tf2::Quaternion quaternion(q[0], q[1], q[2], q[3]);
    tf2::Transform tf2_transform(quaternion, tf2::Vector3());
    tf2_transform.setOrigin(tf2::Vector3(twc.at<float>(0, 2), twc.at<float>(0, 1), twc.at<float>(0, 0)));
    tf2::toMsg(tf2_transform, pose.pose);
    /*
    pose.pose.position.x=new_transform.getOrigin().x();
    pose.pose.position.y=new_transform.getOrigin().y();
    pose.pose.position.z=new_transform.getOrigin().z();
    pose.pose.orientation.x=quaternion.getAxis().x();
    pose.pose.orientation.y=quaternion.getAxis().y();
    pose.pose.orientation.z=quaternion.getAxis().z();
    pose.pose.orientation.w=quaternion.getW();
    */
    pose_pub->publish(pose);
}
