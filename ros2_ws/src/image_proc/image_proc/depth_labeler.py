import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from geometry_msgs.msg import Pose
from cascade_msgs.msg import ImageWithPose
from cv_bridge import CvBridge
from message_filters import ApproximateTimeSynchronizer, Subscriber
import cv2

class DepthLabelerNode(Node):
    def __init__(self):
        super().__init__ ("depth_labeler")
        queue_size=20
        acceptable_delay=0.1 #this is how many seconds of difference we allow between the 2 subscriptions before theyre considered not matching
        tss = ApproximateTimeSynchronizer(
            [Subscriber(self, Image, "/clean_depth_map"),
            Subscriber(self, Image, "/labeled_image"),
            Subscriber(self, Image, "/pose")],
            queue_size,
            acceptable_delay)
        tss.registerCallback(self.synced_callback)
        self.publisher_ = self.create_publisher(ImageWithPose, '/semantic_depth_with_pose', 10)

    def synced_callback(self, depth_msg, label_msg, pose_msg):
        msg=ImageWithPose()
        msg.image=depth_msg #placeholder, will need to actually find overlap between label and depth in future
        msg.pose=pose_msg
        self.publisher_.publish(msg)
        pass

def main(args=None):
    rclpy.init(args=args)
    node = DepthLabelerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
