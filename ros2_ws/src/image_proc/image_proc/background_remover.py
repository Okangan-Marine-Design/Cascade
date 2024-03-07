from . import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

class BackgroundRemoverNode(image_node.ImageNode):
    def __init__(self):
        super().__init__ (sub_topic="/sensors/camera/depth_map",pub_topic="/clean_depth_map", name="background_remover")

def main(args=None):
    rclpy.init(args=args)
    node = BackgroundRemoverNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
