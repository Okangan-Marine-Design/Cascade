import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from sensor_msgs.msg import Imu
from message_filters import ApproximateTimeSynchronizer, Subscriber

class IMUNode(Node):
    def __init__(self):
        super().__init__ ("IMU_processor")
        queue_size=20
        acceptable_delay=0.01 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, SensorReading, "/raw_sensors/imu/angular/x"),
            Subscriber(self, SensorReading, "/raw_sensors/imu/angular/y"),
            Subscriber(self, SensorReading, "/raw_sensors/imu/angular/z"),
            Subscriber(self, SensorReading, "/raw_sensors/imu/linear/x"),
            Subscriber(self, SensorReading, "/raw_sensors/imu/linear/y"),
            Subscriber(self, SensorReading, "/raw_sensors/imu/linear/z")],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.publisher_ = self.create_publisher(Imu, '/sensors/imu', 10)

    def synced_callback(self, ax, ay, az, lx, ly, lz):
        msg=Imu()
        msg.angular_velocity.x=ax.data
        msg.angular_velocity.y=ay.data
        msg.angular_velocity.z=az.data
        msg.linear_acceleration.x=lx.data
        msg.linear_acceleration.y=ly.data
        msg.linear_acceleration.z=lz.data
        msg.header.stamp=self.get_clock().now().to_msg()
        self.publisher_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = IMUNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
