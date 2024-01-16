""" obj_detection/ml.py 
rough file outline for object detection

"""
#TODO: import required libraries

def inference(img){#input: image, output: list of bounding boxes
    objects={};
    #how will a list of objects be represented in a message?
    #maybe one object per message and then theyre published individually to /objects topic
    #example object_msg:
    #  Vector3: relative_position
    #  long: time_found
    #  int: object_type
    return objects
}

def main():
    print('obj_detection node started!')
    #TODO: run infinite loop requesting images from sensor_server
    #TODO: locally run some pretrained model (could just be pretained YOLOv8 to test)

if __name__ == '__main__':
    main()
