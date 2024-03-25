import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from cascade_msgs.msg import Dvl
from geometry_msgs.msg import PoseStamped
from sensor_msgs.msg import Imu
from geometry_msgs.msg import Vector3Stamped
from message_filters import ApproximateTimeSynchronizer, Subscriber

class DeadReckoningNode(Node):
    def __init__(self):
        super().__init__("Dead_Reckoning_Node")
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

    def synced_callback(self, dvl, imu, depth):
        result=PoseStamped()
        nullMsg=SensorReading()
        #add dead reckoning algorithm
        #also publish 6DOF data
        result.header.stamp=self.get_clock().now().to_msg()
        nullMsg.header.stamp=self.get_clock().now().to_msg()
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

