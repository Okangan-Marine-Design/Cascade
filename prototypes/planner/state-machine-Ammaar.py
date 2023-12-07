from transitions import Machine

class Robot(object):
    # list of states representing different states of the robot
    states = [
        'initial_state', 'found_gate', 'reached_gate', 'arrow_state', 
        'found_arrow', 'reached_end_of_arrow', 'reached_buoy', 
        'frequency_state', 'found_frequency', 'reached_frequency', 'end'
    ]

    # list of transitions between states
    # ex: 'search_gate' action allows the robot to transition from 'initial_state' to 'found_gate' only if 'search_gate' is successful
    transitions = [
        ['search_gate', 'initial_state', 'found_gate'],
        ['move_to_gate', 'found_gate', 'reached_gate'],
        ['detect_image', 'reached_gate', 'arrow_state'],
        ['search_arrow', 'arrow_state', 'found_arrow'],
        ['follow_arrow', 'found_arrow', 'reached_end_of_arrow'],
        ['find_buoy', 'reached_end_of_arrow', 'reached_buoy'],
        ['touch_buoy', 'reached_buoy', 'frequency_state'],
        ['search_frequency', 'frequency_state', 'found_frequency'],
        ['move_to_frequency', 'found_frequency', 'reached_frequency'],
        ['surface', 'reached_frequency', 'end'],
        ['fail', '*', 'end'],
    ]

    # initialize the robot class
    def __init__(self):
        self.machine = Machine(
            model=self,
            states=Robot.states,
            transitions=Robot.transitions,
            initial='initial_state',
            after_state_change='on_enter_state'
        )
        
    # method to handle new state
    def on_enter_state(self):
        print("--------------------")
        print(f"Entered {self.state}")
        self.perform_action()
    
    # method to perform action based on current state, which also transitions to a new state
    def perform_action(self):
        # dictionary of actions to perform based on current state
        actions = {
            'initial_state': self.search_gate,
            'found_gate': self.move_to_gate,
            'reached_gate': self.detect_image,
            'arrow_state': self.search_arrow,
            'found_arrow': self.follow_arrow,
            'reached_end_of_arrow': self.find_buoy,
            'reached_buoy': self.touch_buoy,
            'frequency_state': self.search_frequency,
            'found_frequency': self.move_to_frequency,
            'reached_frequency': self.surface,
            'end': self.end,
        }
        # perform action based on current state
        action = actions.get(self.state)
        if action:
            action()
        else:
            print("Action not found for the current state.")
        
    # method to check if the action was successful
    def is_success(self):
        user_input = input("Was the action successful? Enter 1 for Yes, anything else for No: ")
        return user_input.strip() == '1'
    
    # methods to perform actions
    def search_gate(self):
        print("Searching for gate")
        if self.is_success():
            self.trigger('search_gate')
        else:
            self.trigger('fail')

    def move_to_gate(self):
        print("Moving towards gate")
        if self.is_success():
            self.trigger('move_to_gate')
        else:
            self.trigger('fail')

    def detect_image(self):
        print("Detecting image and going through the correct gate")
        if self.is_success():
            self.trigger('detect_image')
        else:
            self.trigger('fail')
            
    def search_arrow(self):
        print("Looking for an arrow")
        if self.is_success():
            self.trigger('search_arrow')
        else:
            self.trigger('fail')

    def follow_arrow(self):
        print("Following arrow")
        if self.is_success():
            self.trigger('follow_arrow')
        else:
            self.trigger('fail')
            
    def find_buoy(self):
        print("Finding buoy and moving towards it")
        if self.is_success():
            self.trigger('find_buoy')
        else:
            self.trigger('fail')
            
    def touch_buoy(self):
        print("Detecting correct image and touching buoy")
        if self.is_success():
            self.trigger('touch_buoy')
        else:
            self.trigger('fail')
            
    def search_frequency(self):
        print("Looking for frequency")
        if self.is_success():
            self.trigger('search_frequency')
        else:
            self.trigger('fail')
            
    def move_to_frequency(self):
        print("Moving towards frequency")
        if self.is_success():
            self.trigger('move_to_frequency')
        else:
            self.trigger('fail')
            
    def surface(self):
        print("Surfacing and ending")
        self.to_end()
            
    def to_end(self):
        self.trigger('surface')
    
    def end(self):
        print("Process ended.")

# create an instance of the Robot class
ogo = Robot()
print(ogo.state)
ogo.perform_action()