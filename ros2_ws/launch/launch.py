from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='hardware_integration',
            executable='serial_intake',
            name='sensor_serial_intake'
        ),
        Node(
            package='hardware_integration',
            executable='json_parser',
            name='sensor_json_parser'
        ),
        Node(
            package='hardware_integration',
            executable='image_publisher',
            name='Intel_D450_RGB'
        ),
        Node(
            package='object_detection',
            executable='yolo',
            name='yolo_object_detector'
        ),
        Node(
            package='image_proc',
            executable='depth_map',
            name='Intel_D450_Depth'
        ),
        Node(
            package='image_proc',
            executable='background_remover',
        ),
        Node(
            package='image_proc',
            executable='depth_labeler',
        ),
        ]
    )
