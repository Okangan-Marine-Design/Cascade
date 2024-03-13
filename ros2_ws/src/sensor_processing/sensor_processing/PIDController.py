import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from message_filters import ApproximateTimeSynchronizer, Subscriber

class PIDNode(Node):
    def __init__(self):
        super().__init__ ("generic_PID_controller")
        queue_size=20
        acceptable_delay=0.1 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, SensorReading, "/PID/XXX/target"),
            Subscriber(self, SensorReading, "/PID/XXX/actual"),
            ],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.publisher_ = self.create_publisher(SensorReading, '/PID_correction/XXX', 10)

    def synced_callback(self, target_msg, actual_msg):
        msg=SensorReading()
        #add PID code
        msg.header.stamp=self.get_clock().now().to_msg()
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = PIDNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
