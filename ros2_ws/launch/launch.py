from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='hardware_integration',
            executable='serial_intake',
            name='serial_intake'
        ),
        Node(
            package='hardware_integration',
            executable='json_parser',
            name='json_parser'
        ),
        Node(
            package='hardware_integration',
            executable='image_publisher',
            name='image_publisher'
        ),
        ]
    )
