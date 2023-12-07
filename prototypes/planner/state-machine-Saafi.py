from transitions import Machine
import time
import random

# Define the Robosub class
class Robosub(object):
    # Define possible states for the state machine
    states = [
        "initial_state", 
        "found_gate", 
        "reached_gate", 
        "arrow_state", 
        "found_arrow", 
        "end_of_arrow", 
        "reached_buoy", 
        "frequency_state", 
        "found_frequency", 
        "reached_frequency",
        "goal",
        "emergency"
    ]

    # Define transitions between states and associated actions
    transitions = [
        ["searching_gate", "initial_state", "found_gate"],
        ["move_to_gate", "found_gate", "reached_gate"],
        ["detect_arrow", "reached_gate", "arrow_state"],
        ["following_arrow", "arrow_state", "found_arrow"],
        ["arrow_path_completed", "found_arrow", "end_of_arrow"],
        ["found_buoy", "end_of_arrow", "reached_buoy"],
        ["searching_frequency", "reached_buoy", "frequency_state"],
        ["follow_frequency", "frequency_state", "found_frequency"],
        ["at_frequency", "found_frequency", "reached_frequency"],
        ["reach_goal", "reached_frequency", "goal"],
        ["emergency_exit", "*", "emergency"]  
    ]

    # Initialize the Robosub class
    def __init__(self):
        # Initialize the state machine with states, transitions, and initial state
        self.machine = Machine(
            model=self,
            states=Robosub.states,
            transitions=Robosub.transitions,
            initial="initial_state",
            after_state_change="on_state_change"
        )

    # Method to handle state transitions
    def set_transitions(self):
        # If the current state is "goal", do nothing and return
        if self.state == "goal":
            return
        # Define actions associated with different states
        actions = {
            "initial_state": self.searching_gate,
            "found_gate": self.move_to_gate,
            "reached_gate": self.detect_arrow,
            "arrow_state": self.following_arrow,
            "found_arrow": self.arrow_path_completed,
            "end_of_arrow": self.found_buoy,
            "reached_buoy": self.searching_frequency,
            "frequency_state": self.follow_frequency,
            "found_frequency": self.at_frequency,
            "reached_frequency": self.reach_goal,
        }
        # Get the action associated with the current state
        action = actions.get(self.state)
        # If there is an action associated with the current state, execute it
        if action:
            action()

    # Method called after each state change
    def on_state_change(self):
        # There is a 10% chance any given action in a state fails and the emergency actions activate
        # If the current state is not "emergency", check if a failure occurs
        if not self.state == "emergency":
            if random.random() < 0.1:
                self.emergency_exit()  # Activate emergency actions with 10% probability
        # Print the current state after each state change
        print("entering", self.state)

    # Method to handle the emergency state
    def emergency_state(self):
        self.emergency_exit()  # Trigger emergency actions

# Create an instance of the Robosub class
sub = Robosub()
print(sub.state)  # Print the initial state of the state machine

# Run the state machine until it reaches either the "goal" or "emergency" state
while sub.state not in ["goal", "emergency"]:
    sub.set_transitions()  # Execute state transitions



# 1. initial state
# -- moving side to side and looking for gate
# 2. found gate
# -- move towards gate
# 3. reached gate
# -- detect image and go through correct gate
# 4. arrow state
# -- looking for an arrow
# 5. found arrow
# -- follow arrow
# 6. reached end of arrow
# -- find buoy and move towards it
# 7. reached buoy
# -- detect correct image and touch it
# 8. frequency state
# -- looking for frequency
# 9. found frequency
# -- move towards frequency
# 10. reached frequency
# -- surface and end
