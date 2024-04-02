import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from cascade_msgs.msg import MotorThrottle
from message_filters import ApproximateTimeSynchronizer, Subscriber

class PIDCombinerNode(Node):
    def __init__(self):
        super().__init__ ("PID_combiner_node")
        queue_size=20
        acceptable_delay=0.01 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, SensorReading, "/PID_correction/yaw"),
                Subscriber(self, SensorReading, "/PID_correction/pitch"),
                Subscriber(self, SensorReading, "/PID_correction/roll"),
                Subscriber(self, SensorReading, "/PID_correction/surge"),
                Subscriber(self, SensorReading, "/PID_correction/sway"),
                Subscriber(self, SensorReading, "/PID_correction/heave"),
            ],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.publisher_ = self.create_publisher(MotorThrottle, '/motor_throttle', 10)

    def synced_callback(self, yaw_msg, pitch_msg, roll_msg, surge_msg, sway_msg, heave_msg):
        motor_msg=MotorThrottle()
        #adding up corresponding pid values for each motor
        motor_msg.fli= heave_msg.data - roll_msg.data - pitch_msg.data
        motor_msg.fri= heave_msg.data + roll_msg.data - pitch_msg.data
        motor_msg.bli= heave_msg.data - roll_msg.data + pitch_msg.data
        motor_msg.bri= heave_msg.data + roll_msg.data + pitch_msg.data

        motor_msg.flo= surge_msg.data - sway_msg.data - yaw_msg.data
        motor_msg.fro= surge_msg.data + sway_msg.data + yaw_msg.data
        motor_msg.blo= -surge_msg.data - sway_msg.data + yaw_msg.data
        motor_msg.bro= -surge_msg.data + sway_msg.data - yaw_msg.data

        self.publisher_.publish(motor_msg)

def main(args=None):
    rclpy.init(args=args)
    node = PIDCombinerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
