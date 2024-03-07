import rclpy
from rclpy.node import Node

class PlannerNode(Node):
    def __init__(self):
        super().__init__ ("Automated_Planner")
        #start planner loop
        
def main(args=None):
    rclpy.init(args=args)
    node = PlannerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
