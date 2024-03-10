from ultralytics import YOLO
import cv2
import cvzone
import math
 
# cap = cv2.VideoCapture(1)  # For Webcam
# cap.set(3, 1280)
# cap.set(4, 720)
cap = cv2.VideoCapture("demo.mov")  # For Video
 
model = YOLO("best.pt")
 
classNames = ["buoy", "gate"]
myColor = (0, 0, 255)

frame_skip = 30 # Number of frames to skip

while True:
    success, img = cap.read()
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
            print(currentClass)
            if conf>0.5:
                if currentClass =='buoy':
                    myColor = (0, 0,255)
                elif currentClass =='gate':
                    myColor =(0,255,0)
                else:
                    myColor = (255, 0, 0)
 
                cvzone.putTextRect(img, f'{classNames[cls]} {conf}',
                                   (max(0, x1), max(35, y1)), scale=1, thickness=1,colorB=myColor,
                                   colorT=(255,255,255),colorR=myColor, offset=5)
                cv2.rectangle(img, (x1, y1), (x2, y2), myColor, 3)
 
    cv2.imshow("Image", img)
    cv2.waitKey(1)
    
    # Skip frames to speed up for demo purposes
    for _ in range(frame_skip):
        cap.read()