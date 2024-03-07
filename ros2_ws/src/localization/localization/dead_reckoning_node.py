import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from cascade_msgs.msg import Dvl
from geometry_msgs.msg import Pose
from sensor_msgs.msg import Imu
from geometry_msgs.msg import Vector3Stamped
from message_filters import ApproximateTimeSynchronizer, Subscriber

class DeadReckoningNode(Node):
    def __init__(self):
        super().__init__("Dead_Reckoning_Node")
        self.publisher_ = self.create_publisher(Pose, "/pose", 10)
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
        self.publisher_ = self.create_publisher(Pose, '/pose', 10)

    def synced_callback(self, dvl, imu, depth):
        result=Pose()
        #add dead reckoning algorithm
        self.publisher_.publish(result)

def main(args=None):
    rclpy.init(args=args)
    node = DeadReckoningNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

