int32 command
geometry_msgs/Vector3 data0 #used to communicate some kind of data along with the command
geometry_msgs/Vector3 data1 

std_msgs/Header header

int32 GO_TO_GATE = 0 
    #goes to gate, using data field 0 to determine how far from gate to stop

int32 MOVE_RELATIVE = 1 
    #does a movement relative to the sub, ex. move 5m forward, rotate 90 degrees cw, etc
    #data0 is for translation (x, y, z)
    #data1 is for rotation (r, p, y) in degrees NOT radians

