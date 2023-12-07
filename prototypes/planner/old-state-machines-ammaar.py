import time
from inputimeout import inputimeout, TimeoutOccurred  # External library to handle input timeouts

# Define the FSM (Finite State Machine) class
class FSM:
    def __init__(self):
        self.states = {}  # Dictionary to store state names and their corresponding functions
        self.current_state = None  # Keep track of the current state the FSM is in
        self.start_time = None  # Store the start time for timeouts

    # Method to add states to the FSM
    def add_state(self, name, function):
        self.states[name] = function

    # Method to set the start state and its starting time
    def set_start(self, name):
        self.current_state = name
        self.start_time = time.time()  # Get the current time

    # Calculate and return the time elapsed from the start
    def time_elapsed(self):
        return time.time() - self.start_time

    # The main loop of the FSM to continuously run the current state's function
    def run(self):
        while self.current_state is not None:  # Keep running as long as there's a current state
            # Call the function corresponding to the current state
            self.current_state = self.states[self.current_state]()

# Dictionary to define timeouts for each state
TIMEOUTS = {
    'gates': 10,
    'bouy': 20,
    'surface': 5
}

# Function to check for user's success condition with a timeout
def success_condition(timeout):
    try:
        # Wait for user input with a timeout
        user_input = inputimeout(prompt="Enter 1 for success, any other key for failure: ", timeout=timeout)
        
        # If the user inputs "1", return True
        if user_input == "1":
            return True
        else :
            print("Task failed. Moving to the next task.\n")
            return False
    except TimeoutOccurred:  # This block executes if the user took too long to respond
        print("Timeout. Moving to the next task.\n")
        return False
    # If any key other than '1' is pressed, return False
    return False

# State function for the "gates" state
def gates_state():
    print("1. Trying to pass through the gate...")
    # If the success condition is met, move to the "bouy" state
    if success_condition(TIMEOUTS['gates']):
        print("Successfully passed through the gate!\n")
        return "bouy"
    return "bouy"  # Default behavior is to move to "bouy"

# State function for the "bouy" state
def bouy_state():
    print("2. Trying to contact the buoy...")
    # If the success condition is met, move to the "surface" state
    if success_condition(TIMEOUTS['bouy']):
        print("Successfully contacted the buoy!\n")
        return "surface"
    return "surface"  # Default behavior is to move to "surface"

# State function for the "surface" state
def surface_state():
    print("3. Trying to surface...")
    # If the success condition is met, the FSM will end (by returning None)
    if success_condition(TIMEOUTS['surface']):
        print("Successfully surfaced!\n")
        return None
    print("Failed to surface. Ending tasks.\n")
    return None  # End the FSM

# Create an FSM instance
fsm = FSM()
# Add states to the FSM
fsm.add_state("gates", gates_state)
fsm.add_state("bouy", bouy_state)
fsm.add_state("surface", surface_state)
# Set the starting state
fsm.set_start("gates")
# Start the FSM loop
fsm.run()