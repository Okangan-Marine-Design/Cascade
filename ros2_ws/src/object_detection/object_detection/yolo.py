from image_proc import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Int16MultiArray
from cv_bridge import CvBridge
import cv2

class YoloNode(image_node.ImageNode):
    def __init__(self):
        super().__init__(sub_topic="/sensors/camera/rgb",  pub_topic="/labeled_image", name="yolo")

    def inference():
        boundingBoxes={}
        #insert yolo inferencing code here
        return boudningBoxes

    #overriding default callback
    def subscription_callback(self, msg):
        self.publish(msg)

def main(args=None):
    rclpy.init(args=args)

    yolo_node = YoloNode()

    rclpy.spin(yolo_node)

    yolo_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
