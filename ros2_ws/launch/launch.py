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
        Node(
            package='mapping',
            executable='octomap_server',
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
        ),
        Node(
            package='navigation',
            executable='navigator',
        ),
        Node(
            package='navigation',
            executable='motion_planner',
        ),
        Node(
            package='hardware_integration',
            executable='motor_cortex',
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='yaw_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/yaw'),
                ('/PID/actual/XXX', '/PID/actual/yaw'),
                ('/PID/correction/XXX', '/PID_correction/yaw')
            ]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='pitch_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/pitch'),
                ('/PID/actual/XXX', '/PID/actual/pitch'),
                ('/PID/correction/XXX', '/PID_correction/pitch')
            ]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='roll_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/roll'),
                ('/PID/actual/XXX', '/PID/actual/roll'),
                ('/PID/correction/XXX', '/PID_correction/roll')
            ]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='surge_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/surge'),
                ('/PID/actual/XXX', '/PID/actual/surge'),
                ('/PID/correction/XXX', '/PID_correction/surge')
            ]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='sway_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/sway'),
                ('/PID/actual/XXX', '/PID/actual/sway'),
                ('/PID/correction/XXX', '/PID_correction/sway')
            ]
        ),
        Node(
            package='sensor_processing',
            executable='pid',
            name='heave_pid_controller',
            remappings=[
                ('/PID/target/XXX', '/PID/target/heave'),
                ('/PID/actual/XXX', '/PID/actual/heave'),
                ('/PID/correction/XXX', '/PID_correction/heave')
            ]
        ),
        ]
    )
