import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
import math
import numpy as np


class ConstraintSolver(Node):

    def __init__(self):
        super().__init__('constraint_solver')

        self.jointb1 = 0.0
        self.jointb3 = 0.0
        self.joint34 = 0.0

        self.subscription = self.create_subscription(
            JointState,
            '/joint_states',
            self.joint_state_callback,
            10
        )

    def joint_state_callback(self, msg):

        joints = dict(zip(msg.name, msg.position))

        self.jointb1 = joints.get('jointb1', None)
        self.jointb3 = joints.get('jointb3', None)
        self.joint34 = joints.get('joint34', None)
    # ---------- Link1 attachment point ----------

        y1_base = 0.2
        z1_base = 0.1

        theta1 = self.jointb1

        p1_local_y = -0.25 * math.sin(theta1)
        p1_local_z =  0.25 * math.cos(theta1)

        p1_y = y1_base + p1_local_y
        p1_z = z1_base + p1_local_z


        # ---------- Link4 attachment point ----------

        theta3 = self.jointb3
        theta4 = self.joint34

        # joint34 location
        j34_y = -0.1 - 0.3 * math.sin(theta3)
        j34_z =  0.1 + 0.3 * math.cos(theta3)

        # link4 tip
        total_angle = theta3 + theta4

        p4_y = j34_y - 0.2 * math.sin(total_angle)
        p4_z = j34_z + 0.2 * math.cos(total_angle)


        # ---------- Error ----------

        dy = p1_y - p4_y
        dz = p1_z - p4_z

        distance = math.sqrt(dy**2 + dz**2)

        self.get_logger().info(
            f'distance = {distance:.4f}'
        )

def main(args=None):
    rclpy.init(args=args)

    node = ConstraintSolver()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()

    if rclpy.ok():
        rclpy.shutdown()

if __name__ == '__main__':
    main()