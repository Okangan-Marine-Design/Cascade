import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

class ImageNode(Node):
    def __init__(self, sub_topic="", pub_topic="", name="Image Node", **kwargs):
        super().__init__(name,**kwargs)
        self.name=name
        self.sub_topic=sub_topic
        self.pub_topic=pub_topic
        if(sub_topic!=""):
            self.subscription = self.create_subscription(
                Image,
                sub_topic,
                self.subscription_callback,
                10)
        if(pub_topic!=""):
            self.publisher_ = self.create_publisher(Image, pub_topic, 10)
        self.bridge = CvBridge()
        self.get_logger().debug('Started Image Node: '+self.name)

    def subscription_callback(self, msg):
        try:
            img = self.bridge.imgmsg_to_cv2(msg, "rgb8")
            #cv2.imshow("data",orig)
            #cv2.waitKey(0);
        except:
            self.get_logger().debug('Failed to convert image msg')

    def publish(self, img):
        try:
            img.header.stamp=self.get_clock().now().to_msg()
            self.publisher_.publish(img)
        except:
            self.get_logger().debug('Failed to send image msg @ '+self.name)


