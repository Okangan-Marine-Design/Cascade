cmake_minimum_required(VERSION 3.5)
project(orbslam3)

# You should set the PYTHONPATH to your own python site-packages path
set(ENV{PYTHONPATH} "/opt/ros/iron/lib/python3.10/site-packages/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
find_package(sensor_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)



find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(message_filters REQUIRED)
find_package(Sophus REQUIRED)
find_package(Pangolin REQUIRED)
find_package(ORB_SLAM3 REQUIRED)
find_package(image_transport REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(slam_messages REQUIRED)

include_directories(
  include
  ${ORB_SLAM3_ROOT_DIR}/include
  ${ORB_SLAM3_ROOT_DIR}/include/CameraModels
)

link_directories(
  include
)

add_executable(mono
  src/monocular/mono.cpp
  src/monocular/monocular-slam-node.cpp
)
ament_target_dependencies(mono rclcpp sensor_msgs geometry_msgs cv_bridge ORB_SLAM3 tf2 tf2_ros Pangolin image_transport tf2_geometry_msgs OpenCV)

add_executable(rgbd
  src/rgbd/rgbd.cpp
  src/rgbd/rgbd-slam-node.cpp
)

ament_target_dependencies(rgbd rclcpp sensor_msgs slam_messages geometry_msgs cv_bridge message_filters tf2 tf2_ros ORB_SLAM3 Pangolin image_transport tf2_geometry_msgs OpenCV)
target_include_directories(rgbd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(rgbd PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
add_executable(stereo
  src/stereo/stereo.cpp
  src/stereo/stereo-slam-node.cpp
)

ament_target_dependencies(stereo rclcpp slam_messages sensor_msgs geometry_msgs cv_bridge message_filters tf2 tf2_ros ORB_SLAM3 Pangolin tf2_geometry_msgs OpenCV)
target_include_directories(stereo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(stereo PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

add_executable(stereo-inertial
 src/stereo-inertial/stereo-inertial.cpp
  src/stereo-inertial/stereo-inertial-node.cpp
)

ament_target_dependencies(stereo-inertial rclcpp sensor_msgs geometry_msgs cv_bridge ORB_SLAM3 tf2 tf2_ros Pangolin image_transport tf2_geometry_msgs OpenCV)

install(TARGETS mono rgbd stereo stereo-inertial
  DESTINATION lib/${PROJECT_NAME})

# Install launch files.
#install(DIRECTORY launch config vocabulary
#  DESTINATION share/${PROJECT_NAME}/)

ament_package()

