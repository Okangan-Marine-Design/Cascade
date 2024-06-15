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
        Node(
            package='mapping',
            executable='mapping_node',
        ),
        Node(
            package='localization',
            executable='dead_reckoning',
        ),
        Node(
            package='sensor_processing',
            executable='dvl',
        ),
        Node(
            package='sensor_processing',
            executable='imu',
        ),
        Node(
            package='sensor_processing',
            executable='water_depth',
        ),
        Node(
            package='mission_planning',
            executable='planner',
            output='screen'
        ),
        Node(
            package='navigation',
            executable='navigator',
        ),
        Node(
            package='navigation',
            executable='motion_planner',
            #prefix=['gdbserver localhost:3000']
            #target remote localhost:3000
        ),
        Node(
            package='navigation',
            executable='motor_cortex',
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='yaw_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/yaw/target'),
                ('/PID/XXX/actual', '/PID/yaw/actual'),
                ('/PID_correction/XXX', '/PID_correction/yaw')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='pitch_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/pitch/target'),
                ('/PID/XXX/actual', '/PID/pitch/actual'),
                ('/PID_correction/XXX', '/PID_correction/pitch')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='roll_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/roll/target'),
                ('/PID/XXX/actual', '/PID/roll/actual'),
                ('/PID_correction/XXX', '/PID_correction/roll')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='surge_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/surge/target'),
                ('/PID/XXX/actual', '/PID/surge/actual'),
                ('/PID_correction/XXX', '/PID_correction/surge')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='sway_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/sway/target'),
                ('/PID/XXX/actual', '/PID/sway/actual'),
                ('/PID_correction/XXX', '/PID_correction/sway')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='heave_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/heave/target'),
                ('/PID/XXX/actual', '/PID/heave/actual'),
                ('/PID_correction/XXX', '/PID_correction/heave')
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='x_translation_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/translation/target'),#goal translation is always 0 for now, so each translational PID controller has the same target
                ('/PID/XXX/actual', '/PID/x_translation/actual'),
                ('/PID_correction/XXX', '/PID/surge/target')#cascading
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='y_translation_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/translation/target'),
                ('/PID/XXX/actual', '/PID/y_translation/actual'),
                ('/PID_correction/XXX', '/PID/sway/target')#cascading
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='z_translation_pid_controller',
            remappings=[
                ('/PID/XXX/target', '/PID/translation/target'),
                ('/PID/XXX/actual', '/PID/z_translation/actual'),
                ('/PID_correction/XXX', '/PID/heave/target')#cascading
            ],
            parameters=[pid_config]
        ),
        Node(
            package='sensor_processing',
            executable='pid_combiner',
        ),
        Node(
            package='sensor_processing',
            executable='sim_adapter',
        ),
        Node(
            package='hardware_integration',
            executable='serial_output',
        ),
        ]
    )
