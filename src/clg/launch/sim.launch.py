from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

import os


def generate_launch_description():

    pkg_path = get_package_share_directory('clg')

    urdf_path = os.path.join(pkg_path, 'model', 'clg.urdf')

    with open(urdf_path, 'r') as file:
        robot_desc = file.read()

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[
            {
                'robot_description': robot_desc,
                'use_sim_time': True
            }
        ],
        output='screen'
    )

    gazebo = ExecuteProcess(
        cmd=['gz', 'sim', '-r'],
        output='screen'
    )

    spawn_robot = TimerAction(
        period=2.0,
        actions=[
            Node(
                package='ros_gz_sim',
                executable='create',
                arguments=[
                    '-topic',
                    'robot_description',
                    '-name',
                    'gripper1'
                ],
                output='screen'
            )
        ]
    )

    joint_state_broadcaster = TimerAction(
        period=5.0,
        actions=[
            ExecuteProcess(
                cmd=[
                    'ros2',
                    'control',
                    'load_controller',
                    '--set-state',
                    'active',
                    'joint_state_broadcaster'
                ],
                output='screen'
            )
        ]
    )

    position_controller = TimerAction(
        period=8.0,
        actions=[
            ExecuteProcess(
                cmd=[
                    'ros2',
                    'control',
                    'load_controller',
                    '--set-state',
                    'active',
                    'position_controller'
                ],
                output='screen'
            )
        ]
    )

    return LaunchDescription([
        robot_state_publisher,
        gazebo,
        spawn_robot,
        joint_state_broadcaster,
        position_controller
    ])