import rclpy
from rclpy.node import Node
from cascade_msgs.msg import MotorThrottle
from geometry_msgs.msg import PoseStamped
from std_msgs.msg import Float64

class SimAdapterNode(Node):
    def __init__(self):
        super().__init__ ("sim_adapter")
        queue_size=20
        acceptable_delay=0.01 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        self.motor_subscription = self.create_subscription(
                MotorThrottle,
                "/motor_throttle",
                self.motor_callback,
                10)
        self.fli = self.create_publisher(Float64, '/cascade/motor_throttle/fli', 10)
        self.flo = self.create_publisher(Float64, '/cascade/motor_throttle/flo', 10)
        self.fri = self.create_publisher(Float64, '/cascade/motor_throttle/fri', 10)
        self.fro = self.create_publisher(Float64, '/cascade/motor_throttle/fro', 10)
        self.bli = self.create_publisher(Float64, '/cascade/motor_throttle/bli', 10)
        self.blo = self.create_publisher(Float64, '/cascade/motor_throttle/blo', 10)
        self.bri = self.create_publisher(Float64, '/cascade/motor_throttle/bri', 10)
        self.bro = self.create_publisher(Float64, '/cascade/motor_throttle/bro', 10)

    def motor_callback(self, motor_msg):
        msg=Float64()

        msg.data=motor_msg.fli;
        self.fli.publish(msg);
        
        msg.data=motor_msg.flo;
        self.flo.publish(msg);
        
        msg.data=motor_msg.fro;
        self.fro.publish(msg);
        
        msg.data=motor_msg.fri;
        self.fri.publish(msg);

        msg.data=motor_msg.bli;
        self.bli.publish(msg);
        
        msg.data=motor_msg.blo;
        self.blo.publish(msg);
        
        msg.data=motor_msg.bro;
        self.bro.publish(msg);
        
        msg.data=motor_msg.bri;
        self.bri.publish(msg);

def main(args=None):
    rclpy.init(args=args)
    node = SimAdapterNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
