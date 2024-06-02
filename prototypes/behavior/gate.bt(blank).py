
import py_trees
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees.composites import Selector
from py_trees.decorators import Retry
from time import sleep


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
        

        
#defining the creating of the behaviour tree
#here I start with the lowest nodes and build up
    
if __name__ == '__main__':

    root = py_trees.composites.Sequence(name = "root", memory = True)
    
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
    decorator1 = Retry(name = "decorator1", child = sequence2, num_failures=6)
    
    selector1 = Selector(name = "selector1", memory = True)
    
    selector1.add_child(found_or_thru1)    
    selector1.add_child(decorator1)
    
    not5mins = Condition("not5mins")
    
    sequence1 = Sequence(name = "sequence1", memory = True)
    
    sequence1.add_child(not5mins)
    sequence1.add_child(selector1)
    sequence1.add_child(sequence3)
    
    root.add_children([sequence1])

    behaviour_tree = py_trees.trees.BehaviourTree(
        root=root
    )
    print(py_trees.display.unicode_tree(root=root))
    behaviour_tree.setup(timeout=15)

    def print_tree(tree):
        print(py_trees.display.unicode_tree(root=tree.root, show_status=True))

    try:
        behaviour_tree.tick_tock(
            period_ms=500,
            number_of_iterations=py_trees.trees.CONTINUOUS_TICK_TOCK,
            pre_tick_handler=None,
            post_tick_handler=print_tree
        )
    except KeyboardInterrupt:
        behaviour_tree.interrupt()
