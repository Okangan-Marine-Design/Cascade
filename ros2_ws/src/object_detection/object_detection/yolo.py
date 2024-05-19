from image_proc import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import cv2
import numpy as np

class YoloNode(image_node.ImageNode):
    def __init__(self):
        super().__init__(sub_topic="/sensors/camera/rgb",  pub_topic="/labeled_image", name="yolo")
        self.bridge = CvBridge()
        # Initialize your YOLO model here

    def inference(self, image):
        boundingBoxes = {}
        # Insert YOLO inferencing code here
        # The boundingBoxes should be a list of dictionaries containing 'class_id', 'confidence', 'x', 'y', 'w', 'h'
        boundingBoxes = [
            {'class_id': 1, 'confidence': 0.95, 'x': 50, 'y': 50, 'w': 100, 'h': 150},
            {'class_id': 2, 'confidence': 0.85, 'x': 200, 'y': 100, 'w': 120, 'h': 160},
            {'class_id': 3, 'confidence': 0.75, 'x': 300, 'y': 200, 'w': 80, 'h': 120},
            {'class_id': 1, 'confidence': 0.90, 'x': 400, 'y': 300, 'w': 150, 'h': 200}
        ]#fake testing data
        return boundingBoxes

    # Overriding default callback
    def subscription_callback(self, msg):
        try:
            # Convert ROS Image message to OpenCV image
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        except CvBridgeError as e:
            self.get_logger().error(f"Failed to convert image: {e}")
            return

        # Run inference
        boundingBoxes = self.inference(cv_image)

        # Create labeled image
        height, width, _ = cv_image.shape
        labeled_image = np.zeros((height, width, 2), dtype=np.float32)

        
        for box in boundingBoxes:
            class_id = box['class_id']
            confidence = box['confidence']
            x, y, w, h = box['x'], box['y'], box['w'], box['h']
            # Apply the most confident class onto the image
            for i in range(x, x + w):
                for j in range(y, y + h):
                    if confidence > labeled_image[j, i, 1]:  # Compare with the stored confidence
                        labeled_image[j, i, 0] = class_id
                    labeled_image[j, i, 1] = confidence

        try:
            # Convert labeled image to ROS Image message
            labeled_msg = self.bridge.cv2_to_imgmsg(labeled_image, encoding="32FC2")
        except CvBridgeError as e:
            self.get_logger().error(f"Failed to convert labeled image: {e}")
            return

        # Publish the labeled image
        self.publisher_.publish(labeled_msg)

def main(args=None):
    rclpy.init(args=args)
    yolo_node = YoloNode()
    rclpy.spin(yolo_node)
    yolo_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

