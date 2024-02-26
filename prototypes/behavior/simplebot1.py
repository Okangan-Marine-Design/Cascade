from time import sleep
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees import logging


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
        
        

def make_bt():
    root = Sequence(name="sequence", memory = True)
    
    checkbattery = Condition("check_battery")
    approach_object = Action("approach_object")
    complete_task = Action("complete_task")
    
    root.add_children(
        [
            checkbattery,
            approach_object,
            complete_task,
        ]
    )
    
    return root
    
if __name__ == "__main__":
    logging.level = logging.level.DEBUG
    tree = make_bt()
    tree.tick_once()