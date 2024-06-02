from . import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

class DepthMapNode(image_node.ImageNode):
    def __init__(self):
        super().__init__ (pub_topic="/sensors/camera/depth_map", name="depth_map_pub")

def main(args=None):
    rclpy.init(args=args)
    node = DepthMapNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

##probably a temp file, something else will publish depth map data
