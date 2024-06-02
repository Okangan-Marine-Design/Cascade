import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from cascade_msgs.msg import Dvl
from message_filters import ApproximateTimeSynchronizer, Subscriber

class DVLNode(Node):
    def __init__(self):
        super().__init__ ("DVL_processor")
        queue_size=20
        acceptable_delay=0.1 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, SensorReading, "/raw_sensors/dvl/velocity/x"),
            Subscriber(self, SensorReading, "/raw_sensors/dvl/velocity/y"),
            Subscriber(self, SensorReading, "/raw_sensors/dvl/velocity/z")],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.publisher_ = self.create_publisher(Dvl, '/sensors/dvl', 10)

    def synced_callback(self, x, y, z):
        msg=Dvl()
        msg.x=x.data
        msg.y=y.data
        msg.z=z.data
        msg.header.stamp=self.get_clock().now().to_msg()
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = DVLNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
