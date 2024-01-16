#include "rgbd-slam-node.hpp"

#include <opencv2/core/core.hpp>

using std::placeholders::_1;

RgbdSlamNode::RgbdSlamNode(ORB_SLAM3::System* pSLAM)
:   Node("ORB_SLAM3_ROS2"){
    m_SLAM = pSLAM;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pose_pub = 
        node->create_publisher<geometry_msgs::PoseStamped>("/pose", 10);
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
    geometry_msgs::PoseStamped pose;
    pose.header.stamp = ros::Time::now();
    pose.header.frame_id ="map";

    cv::Mat Rwc = Tcw.rowRange(0,3).colRange(0,3).t(); // Rotation information
    cv::Mat twc = -Rwc*Tcw.rowRange(0,3).col(3); // translation information
    vector<float> q = ORB_SLAM2::Converter::toQuaternion(Rwc);

    tf::Transform new_transform;
    new_transform.setOrigin(tf::Vector3(twc.at<float>(0, 0), twc.at<float>(0, 1), twc.at<float>(0, 2)));

    tf::Quaternion quaternion(q[0], q[1], q[2], q[3]);
    new_transform.setRotation(quaternion);

    tf::poseTFToMsg(new_transform, pose.pose);
    pose_pub.publish(pose);
}
