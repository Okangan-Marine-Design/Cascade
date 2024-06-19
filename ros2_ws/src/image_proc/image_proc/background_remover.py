

import cv2
import numpy as np
from cv_bridge import CvBridge
from sensor_msgs.msg import Image
import rclpy
from rclpy.node import Node

class BackgroundRemoverNode(Node):
    def __init__(self):
        super().__init__("background_remover")
        self.pub_topic = "/clean_depth_map"
        self.bridge = CvBridge()
        self.subscription = self.create_subscription(
            Image,
            "/sensors/camera/depth_map",
            self.subscription_callback,
            10
        )
        self.publisher = self.create_publisher(Image, self.pub_topic, 10)

    def adaptive_thresholding(self, depth_image):
        # Check if the image is already a grayscale image
        if len(depth_image.shape) == 2:
            gray_image = depth_image  # it is already grayscale
        else:
            gray_image = cv2.cvtColor(depth_image, cv2.COLOR_BGR2GRAY)  # Convert to grayscale

        # Apply adaptive thresholding
        thresholded_image = cv2.adaptiveThreshold(
            gray_image, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
            cv2.THRESH_BINARY, 11, 2
        )
        return thresholded_image

    def subscription_callback(self, msg):
        # Convert ROS image message to OpenCV image
        cv_depth_image = self.bridge.imgmsg_to_cv2(msg, "16UC1")

        # Apply adaptive thresholding
        mask = self.adaptive_thresholding(cv_depth_image)

        # Bitwise operation to get the foreground
        foreground = cv2.bitwise_and(cv_depth_image, cv_depth_image, mask=mask)

        # Convert the result to ROS image message
        clean_depth_image_msg = self.bridge.cv2_to_imgmsg(foreground, encoding="16UC1")

        # Publish the processed image
        self.publisher.publish(clean_depth_image_msg)

def main(args=None):
    rclpy.init(args=args)
    node = BackgroundRemoverNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

