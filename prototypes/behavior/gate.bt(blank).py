from time import sleep
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees.composites import Selector
from py_trees import logging
from py_trees.decorators import Decorator 


#This is a blank version of the behavior tree for the prequalifier gate
#You can run this code to see how it goes through the behavior tree
#Just note that running it this way does not return failure on any of the nodes
#Each action and condition is currently left blank without a function











#Defining an action
class Action(Behaviour): 
    def __init__(self,name):
        super(Action,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"Action::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"Action::initialise{self.name}")

    def update(self):
        self.logger.debug(f"Action::update{self.name}")
        sleep(1)
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug(f"Action::terminate{self.name} to {new_status}")
        
        
#Defining a condition
class Condition(Behaviour):
    def __init__(self,name):
        super(Condition,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"Condition::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"Condition::initialise{self.name}")

    def update(self):
        self.logger.debug(f"Condition::update{self.name}")
        sleep(1)
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug(f"Condition::terminate{self.name} to {new_status}")
        
#This is defining a Decorator node that allows a child node to fail a certain number of times before returning failure to the parent
#There are still other functionalities to a decorator node.
class MaxFailures(Decorator):

    def __init__(self, name, max_failures, child):
       
        super(MaxFailures, self).__init__(name=name, child=child)
        self.max_failures = max_failures
        self.failures_count = 0

    def update(self):
    
        # Update the child node
        status = self.children[0].update()

        # If child node fails, increment the failures count
        if status == Status.FAILURE:
            self.failures_count += 1

        # If the number of failures exceeds the maximum allowed, return FAILURE
        if self.failures_count >= self.max_failures:
            return Status.FAILURE

        # Otherwise, return the status of the child node
        return status
        
        
#defining the creating of the behaviour tree
#here I start with the lowest nodes and build up
def make_bt():
    root = Sequence(name="sequence", memory = True)
    
    orient = Action("orient")
    go_thru = Action("go_thru")
    
    sequence3 = Sequence(name = "sequence3", memory = True)
    sequence3.add_child(orient)
    sequence3.add_child(go_thru)
    
    up_and_down = Action("up_and_down")
    rotate = Action("rotate")
    side_to_side = Action("side_to_side")
    found_gate = Condition("found_gate")
    
    sequence2 = Sequence(name = "sequence2", memory = True)
    sequence2.add_child(up_and_down)
    sequence2.add_child(rotate)
    sequence2.add_child(side_to_side)
    sequence2.add_child(found_gate)
    
    found_or_thru1 = Condition("found_or_thru1")
    decorator1 = MaxFailures("decorator1", max_failures = 6, child = sequence2)
    
    selector1 = Selector(name = "selector1", memory = True)
    
    selector1.add_child(found_or_thru1)    
    selector1.add_child(decorator1)
    
    not5mins = Condition("not5mins")
    
    sequence1 = Sequence(name = "sequence1", memory = True)
    
    sequence1.add_child(not5mins)
    sequence1.add_child(selector1)
    sequence1.add_child(sequence3)
    
    
    
    root.add_children(
        [
            sequence1
        ]
    )
    
    return root
    
if __name__ == "__main__":
    logging.level = logging.level.DEBUG
    tree = make_bt()
    tree.tick_once()