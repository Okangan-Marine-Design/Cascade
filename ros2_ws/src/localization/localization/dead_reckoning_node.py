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

    def sim_pose_callback(self, pose_msg):
        self.sim_mode=True
        result=PoseStamped()
        result.pose=pose_msg
        q=pose_msg.orientation
        eulers=self.euler_from_quaternion(q.x,q.y,q.z,q.w)

        msg=SensorReading()
        result.header.stamp=self.get_clock().now().to_msg()
        msg.header.stamp=self.get_clock().now().to_msg()

        msg.data=eulers[2]
        self.pidPublisherMap["yaw"].publish(msg);

        msg.data=eulers[1]
        self.pidPublisherMap["pitch"].publish(msg);

        msg.data=eulers[0]
        self.pidPublisherMap["roll"].publish(msg);

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

