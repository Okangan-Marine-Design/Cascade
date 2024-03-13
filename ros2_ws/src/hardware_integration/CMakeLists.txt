cmake_minimum_required(VERSION 3.8)
project(hardware_integration)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cascade_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)

add_executable(serial_intake
  src/serial_intake.cpp)

ament_target_dependencies(serial_intake rclcpp std_msgs cascade_msgs)

add_executable(json_parser
  src/json_parser.cpp)

ament_target_dependencies(json_parser rclcpp std_msgs cascade_msgs)


add_executable(image_publisher
  src/image_publisher.cpp)

ament_target_dependencies(image_publisher rclcpp std_msgs sensor_msgs cv_bridge OpenCV cascade_msgs)

install(TARGETS
  serial_intake
  json_parser
  image_publisher
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
