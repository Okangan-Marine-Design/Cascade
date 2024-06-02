import rclpy
from rclpy.node import Node
from cascade_msgs.msg import MovementCommand
from cascade_msgs.msg import Status

class PlannerNode(Node):
    def __init__(self):
        super().__init__ ("Automated_Planner")
        self.publisher_ = self.create_publisher(MovementCommand, "/movement_command", 10)
        self.subscription = self.create_subscription(
                Status,
                "movement_command_status",
                self.subscription_callback,
                10)
        #start planner loop
    
    def publish(self, msg):
        self.publisher_.publish(msg)

    def subscription_callback(self, msg):
        pass

def main(args=None):
    rclpy.init(args=args)
    node = PlannerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
