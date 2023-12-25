cmake_minimum_required(VERSION 3.8)
project(example)

# You should set the PYTHONPATH to your own python site-packages path
set(ENV{PYTHONPATH} "/opt/ros/iron/lib/python3.10/site-packages/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
find_package(rosidl_default_generators REQUIRED)

# example packages to be included
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(message_filters REQUIRED)
find_package(Sophus REQUIRED)
find_package(Pangolin REQUIRED)
find_package(image_transport REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(robo_messages REQUIRED)

# directories to include
include_directories(
  include
)

link_directories(
  include
)

add_executable(node
  src/node.cpp
)
ament_target_dependencies(mono rclcpp sensor_msgs geometry_msgs cv_bridge ORB_SLAM3 tf2 tf2_ros Pangolin image_transport tf2_geometry_msgs OpenCV)

# The following code fixed some things at some point?
#
# target_include_directories(rgbd PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#   $<INSTALL_INTERFACE:include>)
# target_compile_features(rgbd PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS node DESTINATION lib/${PROJECT_NAME})

ament_package()
