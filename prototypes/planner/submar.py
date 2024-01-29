depth = 0
velocity = 0
battery = 100

def desc(depth,delta_depth):     #m and m/s respectively
    while depth > -20 :
        delta_depth = -1
        depth += delta_depth
        if depth == -20 :
            delta_depth = 0

def objective_search(objective, velocity):
    while objective == False :
        velocity = 10
    else:
        velocity = 0

def asc(depth, delta_depth):
    while depth < 0 :
        delta_depth = 1
        depth += delta_depth
        if depth == 0 :
            delta_depth = 0

while battery > 5 :
    desc(depth,delta_depth=0)
    objective_search(objective=False,velocity=0)
    print(depth)
else:
    print("Battery low, ascending")
    asc(depth, delta_depth=0)
    print(depth)

