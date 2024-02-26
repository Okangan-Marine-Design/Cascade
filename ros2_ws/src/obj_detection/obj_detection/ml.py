""" obj_detection/ml.py 
rough file outline for object detection

"""
import rclpy
from rclpy.node import Node
from ultralytics import YOLO
import cv2
import cvzone
import math
from sensor_msgs.msg import Image
from rclpy.qos import qos_profile_sensor_data
#TODO: import required libraries

model = YOLO("best.pt")
 
classNames = ["buoy_bootlegger", "buoy_gman", "gate_bootlegger", "gate_gman", "test1", "test2", "test3", "test4", "test5", "test6", "test7", "test8", "test9", "test10", "test11" ]

def listener_callback(self):
        print("In callback")

def inference(img):#input: image, output: list of bounding boxes
    objects={};
    #how will a list of objects be represented in a message?
    #maybe one object per message and then theyre published individually to /objects topic
    #example object_msg:
    #  Vector3: relative_position
    #  long: time_found
    #  int: object_type
    return objects


def main():
    print('obj_detection node started!')
    self.subscription = self.create_subscription(
        Image,
        '/camera/front/rgb',
        self.listener_callback,
        qos_profile_sensor_data)
    '''
    while True:
    #img=a
    results = model(img, stream=True)
    for r in results:
        boxes = r.boxes
        for box in boxes:
            # Bounding Box
            x1, y1, x2, y2 = box.xyxy[0]
            x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
            # cv2.rectangle(img,(x1,y1),(x2,y2),(255,0,255),3)
            w, h = x2 - x1, y2 - y1
            # cvzone.cornerRect(img, (x1, y1, w, h))
 
            # Confidence
            conf = math.ceil((box.conf[0] * 100)) / 100
            # Class Name
            cls = int(box.cls[0])
            currentClass = classNames[cls]
    '''
 
if __name__ == '__main__':
    main()

