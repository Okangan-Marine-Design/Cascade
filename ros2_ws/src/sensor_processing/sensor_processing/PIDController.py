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
        self.declare_parameter('max', 0.0)
        self.kD=0.1
        self.kI=0.01
        self.kP=1.0
        self.bias=0.0
        self.max=0.0
        self.paramsRead=False
        self.saturation_limit=100
        self.I=0.0
        self.wrap=0
        self.prevMsg=SensorReading()
        
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
             self.max = self.get_parameter('max').get_parameter_value().double_value
             self.paramsRead=True

        msg=SensorReading()

        # Detect angle wrap-around, should never happen for surge sway heave controllers because the ranges of those are much lower
        if (actual_msg.data - self.prevMsg.data < -300.0):
            self.wrap+=1
        elif (actual_msg.data - self.prevMsg.data > 300.0):
            self.wrap-=1
        
        target = target_msg.data
        actual = (actual_msg.data+self.wrap*360.0)
        
        error=target-actual

        if(abs(error+360)<abs(error)):
            error+=360

        if(abs(error-360)<abs(error)):
            error-=360
        
        if(self.lastMsgTime>0):
            dt=(self.get_clock().now().nanoseconds-self.lastMsgTime)/1000000000.0
            #dt is time delta from last message in seconds 
            P=self.kP*error
            self.I+=self.kI*error*dt
            self.I = max(min(self.I, self.saturation_limit), -self.saturation_limit)
            #TODO: turn saturation into a ros2 parameter
            D=self.kD*(error-self.lastError)/dt
            if(abs(self.max)>0):
                msg.data=min(abs(P+self.I+D),abs(self.max))
                if(P+self.I+D<0):
                    msg.data*=-1
            else:
                msg.data=P+self.I+D
        else:
            msg.data=0.0

        self.lastError=error
        self.prevMsg=actual_msg
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
