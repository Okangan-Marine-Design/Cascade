depth = 0                 # Defining intitial conditions
velocity = 0
battery = 100
objective = False
objectiveclass = None

while battery > 5:        # Only operating above a certain battery percentage
    while depth > -20 :   #Descending to desired depth
        delta_depth = -1
        depth += delta_depth
    if depth == -20 :
        delta_depth = 0

    while objective == False :       #Object searching
        velocity = 10
        objectiveclass = str(input("object type?"))   #Use manual input to simulate identifying an object
        if objectiveclass != "None":
            objective = True
    else:
        velocity = 0

    if objectiveclass == "Choose Your Side":      #Define the tasks for each objective
        print(1)                                  #Will probably call a function for each task

    if objectiveclass == "Path":
        print(2)

    if objectiveclass == "Make The Grade":
        print(3)

    if objectiveclass == "Collecting":
        print(4)

    if objectiveclass == "Survive the Shootout":
        print(5)

    if objectiveclass == "Cash Or Smash":
        print(6)

