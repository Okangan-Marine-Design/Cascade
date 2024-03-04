from . import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
from message_filters import ApproximateTimeSynchronizer, Subscriber
import cv2

class DepthLabelerNode(image_node.ImageNode):
    def __init__(self):
        super().__init__ (name="depth_labeler",pub_topic='/semantic_depth')
        queue_size=20
        acceptable_delay=0.1 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, Image, "/clean_depth_map"),
            Subscriber(self, Image, "/labeled_image")],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)

    def synced_callback(self, depth_msg, label_msg):
        #combine the depth and labels
        pass

def main(args=None):
    rclpy.init(args=args)
    node = DepthLabelerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
