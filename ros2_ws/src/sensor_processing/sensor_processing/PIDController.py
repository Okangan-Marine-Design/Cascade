import rclpy
from rclpy.node import Node
from cascade_msgs.msg import SensorReading
from message_filters import ApproximateTimeSynchronizer, Subscriber

class PIDNode(Node):
    def __init__(self):
        super().__init__ ("generic_PID_controller")
        self.declare_parameter('bias', 0.0)
        self.declare_parameter('kD', 0.0)
        self.declare_parameter('kI', 0.0)
        self.declare_parameter('kP', 0.0)
        self.kD=0.1
        self.kI=0.01
        self.kP=1.0
        self.bias=0.0
        self.paramsRead=False
        self.I=0.0
        
        self.lastMsgTime=-1
        self.lastError=0.0

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
        if(self.paramsRead==False):
             self.kI = self.get_parameter('kI').get_parameter_value().double_value
             self.kP = self.get_parameter('kP').get_parameter_value().double_value
             self.kD = self.get_parameter('kD').get_parameter_value().double_value
             self.bias = self.get_parameter('bias').get_parameter_value().double_value
             self.paramsRead=True

        msg=SensorReading()

        error=target_msg.data-actual_msg.data

        if(self.lastMsgTime>0):
            dt=(self.get_clock().now().nanoseconds-self.lastMsgTime)/1000000000.0
            #dt is time delta from last message in seconds 
            P=self.kP*error
            self.I+=self.kI*error*dt
            D=self.kD*(error-self.lastError)/dt
            msg.data=P+self.I+D
        else:
            msg.data=0.0

        self.lastError=error
        self.lastMsgTime=self.get_clock().now().nanoseconds
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
