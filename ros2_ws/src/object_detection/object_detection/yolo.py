from image_proc import image_node
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import cv2
import numpy as np
import torch
from pathlib import Path
from utils.common import DetectMultiBackend
from utils.general import non_max_suppression, scale_boxes
from utils.torch_utils import select_device

class YoloNode(image_node.ImageNode):
    def __init__(self):
        super().__init__(sub_topic="/sensors/camera/rgb", pub_topic="/labeled_image", name="yolo")
        self.bridge = CvBridge()
        
        # Initialize YOLO model
        weights = 'best.pt'  # Path to your model weights
        device = select_device('')  # Select device, '' means auto (CPU or CUDA)
        self.model = DetectMultiBackend(weights, device=device)
        self.model.warmup(imgsz=(1, 3, 320, 320))  # Adjust the size as per your model
        self.names = self.model.names

    def inference(self, image):
        # Preprocess the image
        img = cv2.resize(image, (320, 320))  # Adjust the size as per your model
        cv2.imshow("raw image",img)
        cv2.waitKey(1)  # Add a small delay to allow the image to be displayed
        img = img.transpose((2, 0, 1))  # HWC to CHW
        img = np.expand_dims(img, axis=0)  # Add batch dimension
        img = torch.from_numpy(img).to(self.model.device)
        img = img.float() / 255.0  # Normalize to [0, 1]

        # Run inference
        pred = self.model(img)

        # Debug prints for prediction
        self.get_logger().info(f'Prediction shape: {pred.shape}')
        self.get_logger().info(f'Prediction: {pred[0]}')

        # Apply Non-Maximum Suppression (NMS)
        pred = non_max_suppression(pred, 0.25, 0.45, classes=None, agnostic=False)

        # Debug prints for NMS
        print(f'Predictions after NMS: {pred}')

        # Process predictions
        boundingBoxes = []
        for det in pred:  # detections per image
            if len(det):
                det[:, :4] = scale_boxes(img.shape[2:], det[:, :4], image.shape).round()
                for *xyxy, conf, cls in reversed(det):
                    x1, y1, x2, y2 = xyxy
                    w, h = x2 - x1, y2 - y1
                    boundingBoxes.append({
                        'class_id': int(cls)+1,
                        'confidence': float(conf),
                        'x': int(x1),
                        'y': int(y1),
                        'w': int(w),
                        'h': int(h)
                    })
        return boundingBoxes

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
            x = max(0, min(x, width - 1))
            y = max(0, min(y, height - 1))
            w = max(0, min(w, width - x))
            h = max(0, min(h, height - y))
            
            # Create slices for the region of interest
            roi_x = slice(x, x + w)
            roi_y = slice(y, y + h)

            # Update class and confidence images using NumPy's vectorized operations
            mask = confidence > labeled_image[roi_y, roi_x, 1]
            labeled_image[roi_y, roi_x, 0] = np.where(mask, class_id, labeled_image[roi_y, roi_x, 0])
            labeled_image[roi_y, roi_x, 1] = np.where(mask, confidence, labeled_image[roi_y, roi_x, 1])

        try:
            # Convert labeled image to ROS Image message
            labeled_msg = self.bridge.cv2_to_imgmsg(labeled_image, encoding="32FC2")
        except CvBridgeError as e:
            self.get_logger().error(f"Failed to convert labeled image: {e}")
            return

        # Publish the labeled image
        labeled_msg.header.stamp = self.get_clock().now().to_msg()
        self.publisher_.publish(labeled_msg)

def main(args=None):
    rclpy.init(args=args)
    yolo_node = YoloNode()
    rclpy.spin(yolo_node)
    yolo_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
