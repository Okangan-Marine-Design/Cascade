from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    pid_config = os.path.join(
      'config',
      'pid.yaml'
    )

    return LaunchDescription([
        
        Node(
            package='object_detection',
            executable='yolo',
            name='yolo_object_detector'
        ),
        Node(
            package='image_proc',
            executable='background_remover',
        ),
        Node(
            package='image_proc',
            executable='depth_labeler',
        ),
        Node(
            package='mapping',
            executable='mapping_node',
        ),
        Node(
            package='localization',
            executable='dead_reckoning',
        ),
        Node(
            package='mapping',
            executable='visualizer',
            arguments=['map.bx','--live']
        ),
        ]
    )
