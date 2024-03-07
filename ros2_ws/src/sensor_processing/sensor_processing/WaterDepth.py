import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading

class WaterDepthNode(Node):
    def __init__(self):
        super().__init__ ("water_depth_processor")
        queue_size=20
        acceptable_delay=0.01 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        self.subscription = self.create_subscription(
                SensorReading,
                "/raw_sensors/depth",
                self.subscription_callback,
                10)
        self.publisher_ = self.create_publisher(SensorReading, '/sensors/depth', 10)

    def subscription_callback(self, depth_msg):
        msg=SensorReading()
        msg.data=depth_msg.data;
        #do some kind of smoothing or backup plan here in case readings are whack
        msg.header.stamp=self.get_clock().now().to_msg()
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = WaterDepthNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
