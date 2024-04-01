import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from cascade_msgs.msg import Dvl
from geometry_msgs.msg import PoseStamped
from geometry_msgs.msg import Pose
from geometry_msgs.msg import Quaternion
from sensor_msgs.msg import Imu
from geometry_msgs.msg import Vector3Stamped
from message_filters import ApproximateTimeSynchronizer, Subscriber
import math
import tf2_ros
import numpy as np

class DeadReckoningNode(Node):
    def __init__(self):
        super().__init__("Dead_Reckoning_Node")
        self.sim_mode=False
        self.pose_publisher_ = self.create_publisher(PoseStamped, "/pose", 10)
        self.pidPublisherMap={}
        self.pidPublisherMap["yaw"] = self.create_publisher(SensorReading, "/PID/yaw/actual", 10)
        self.pidPublisherMap["pitch"] = self.create_publisher(SensorReading, "/PID/pitch/actual", 10)
        self.pidPublisherMap["roll"] = self.create_publisher(SensorReading, "/PID/roll/actual", 10)
        self.pidPublisherMap["surge"] = self.create_publisher(SensorReading, "/PID/surge/actual", 10)
        self.pidPublisherMap["sway"] = self.create_publisher(SensorReading, "/PID/sway/actual", 10)
        self.pidPublisherMap["heave"] = self.create_publisher(SensorReading, "/PID/heave/actual", 10)
        self.get_logger().debug('Started Dead_Reckoning_Node')
        queue_size=20
        acceptable_delay=0.1 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, Dvl, "/sensors/dvl"),
            Subscriber(self, Imu, "/sensors/imu"),
            Subscriber(self, SensorReading, "/sensors/depth")
                ],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.sim_pose_subscription = self.create_subscription(
                Pose,
                "/model/cascade/pose",
                self.sim_pose_callback,
                10)
        self.previous_pose=PoseStamped()

    def euler_from_quaternion(self, x, y, z, w):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        t0 = +2.0 * (w * x + y * z)
        t1 = +1.0 - 2.0 * (x * x + y * y)
        roll_x = math.atan2(t0, t1)
     
        t2 = +2.0 * (w * y - z * x)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)
     
        t3 = +2.0 * (w * z + x * y)
        t4 = +1.0 - 2.0 * (y * y + z * z)
        yaw_z = math.atan2(t3, t4)
     
        return roll_x*(180./math.pi), pitch_y*(180./math.pi), yaw_z*(180./math.pi) 
    
    def quaternion_to_rotation_matrix(self, quaternion):
        x, y, z, w = quaternion
        return np.array([
            [1 - 2*(y**2 + z**2), 2*(x*y - z*w), 2*(x*z + y*w)],
            [2*(x*y + z*w), 1 - 2*(x**2 + z**2), 2*(y*z - x*w)],
            [2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x**2 + y**2)]
        ])

    def surge_sway_heave_from_pose(self, msg):
        surge=sway=heave=0

        if self.previous_pose is not None:
            try:
                # Calculate pose differences (displacements) between consecutive poses
                current_time = msg.header.stamp.sec + msg.header.stamp.nanosec * 1e-9
                previous_time = self.previous_pose.header.stamp.sec + self.previous_pose.header.stamp.nanosec * 1e-9

                # Compute the time difference between the current and previous messages
                dt = current_time - previous_time
                dx = msg.pose.position.x - self.previous_pose.pose.position.x
                dy = msg.pose.position.y - self.previous_pose.pose.position.y
                dz = msg.pose.position.z - self.previous_pose.pose.position.z

                # Extract orientation (quaternion) from current pose
                orientation = [
                    msg.pose.orientation.x,
                    msg.pose.orientation.y,
                    msg.pose.orientation.z,
                    msg.pose.orientation.w
                ]

            # Convert quaternion to rotation matrix
                R_world_to_robot = self.quaternion_to_rotation_matrix(orientation)

                # Transform pose differences into robot's local coordinate frame
                displacement_world_frame = np.array([dx, dy, dz])
                displacement_robot_frame = np.dot(R_world_to_robot.T, displacement_world_frame)

            # Extract surge, sway, and heave components in robot's frame of reference
                surge = displacement_robot_frame[0]/dt
                sway = displacement_robot_frame[1]/dt
                heave = displacement_robot_frame[2]/dt

            # Do something with surge, sway, and heave...

            except Exception as e:
                pass

        # Update previous pose for next iteration
        self.previous_pose = msg
        return [0.0,0.0,0.0]
        return [surge,sway,heave]

    def sim_pose_callback(self, pose_msg):
        self.sim_mode=True
        result=PoseStamped()
        result.pose=pose_msg
        q=pose_msg.orientation
        result.header.stamp=self.get_clock().now().to_msg()

        eulers=self.euler_from_quaternion(q.x,q.y,q.z,q.w)
        ssh=self.surge_sway_heave_from_pose(result)

        msg=SensorReading()
        msg.header.stamp=self.get_clock().now().to_msg()

        msg.data=eulers[2]
        self.pidPublisherMap["yaw"].publish(msg)

        msg.data=eulers[1]
        self.pidPublisherMap["pitch"].publish(msg)

        msg.data=eulers[0]
        self.pidPublisherMap["roll"].publish(msg)

        msg.data=ssh[0]
        self.pidPublisherMap["surge"].publish(msg)

        msg.data=ssh[1]
        self.pidPublisherMap["sway"].publish(msg)
        
        msg.data=ssh[2]
        self.pidPublisherMap["heave"].publish(msg)

        self.pose_publisher_.publish(result)

    def synced_callback(self, dvl, imu, depth):
        result=PoseStamped()
        nullMsg=SensorReading()
        #add dead reckoning algorithm
        #also publish 6DOF data
        result.header.stamp=self.get_clock().now().to_msg()
        nullMsg.header.stamp=self.get_clock().now().to_msg()
        
        if(self.sim_mode==False):
            self.pidPublisherMap["yaw"].publish(nullMsg);
            self.pidPublisherMap["pitch"].publish(nullMsg);
            self.pidPublisherMap["roll"].publish(nullMsg);
            self.pidPublisherMap["surge"].publish(nullMsg);
            self.pidPublisherMap["sway"].publish(nullMsg);
            self.pidPublisherMap["heave"].publish(nullMsg);
            self.pose_publisher_.publish(result)

def main(args=None):
    rclpy.init(args=args)
    node = DeadReckoningNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

