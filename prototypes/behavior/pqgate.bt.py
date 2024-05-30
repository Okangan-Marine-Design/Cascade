import py_trees
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees.composites import Selector
from py_trees.decorators import Retry
from py_trees.decorators import Repeat
from py_trees import display  



#Defining the behaviours for locomotion


class move_1m(Behaviour): 
    def __init__(self,name):
        super(move_1m,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"move_1m::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"move_1m::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [move_1m::update()]" % self.name)
        print("command 1m_forward")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [move_1m::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
class rise_1m(Behaviour): 
    def __init__(self,name):
        super(rise_1m,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"rise_1m::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"rise_1m::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [rise_1m::update()]" % self.name)
        print("command 1m_rise")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [rise_1m::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
class fall_1m(Behaviour): 
    def __init__(self,name):
        super(fall_1m,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"fall_1m::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"fall_1m::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [fall_1m::update()]" % self.name)
        print("command 1m_fall")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [fall_1m::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
class turn90cw(Behaviour): 
    def __init__(self,name):
        super(turn90cw,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"turn90cw::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"turn90cw::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [turn90cw::update()]" % self.name)
        print("command turn 90 clockwise")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [turn90cw::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        

#This behavior is for moving to the currently identified object
class move_to_io(Behaviour): 
    def __init__(self,name):
        super(move_to_io,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"move_to_io::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"move_to_io::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [move_to_io::update()]" % self.name)
        print("move_to_io")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [move_to_io::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
#Defining a condition
class io_is_gate(Behaviour):
    def __init__(self,name):
        super(io_is_gate,self).__init__(name)

    def setup(self):
        self.logger.debug(f"io_is_gate::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"io_is_gate::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [io_is_gate::update()]" % self.name)
        if 1 == 1:     #replace later with if io(identified object) == "gate"
            return Status.SUCCESS
        else:
            return Status.FAILURE
    
    def terminate(self, new_status):
        self.logger.debug("  %s [io_is_gate::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
        
        
#defining the creating of the behaviour tree
#here I start with the lowest nodes and build up
if __name__ == '__main__':

    root = py_trees.composites.Sequence(name = "root", memory = True)
    
    forward = move_1m("forward")
    rise = rise_1m("rise")
    fall = fall_1m("fall")
    move_to = move_to_io("move_to")
    turn90 = turn90cw("turn90")
    
    found_gate = io_is_gate("found_gate")
    
    move_tosq4 = move_to_io("move_tosq4")
    forwardsq4 = move_1m("forwardsq4")
    
    sequence4 = Sequence(name = "sequence4", memory = True)
    sequence4.add_child(move_tosq4)
    sequence4.add_child(forwardsq4)
    
    
    
    fallsq2 = fall_1m("fallsq2")
    turn90sq2 = turn90cw("turn90sq2")
    found_gatesq2 = io_is_gate("found_gatesq2")
    
    sequence2 = Sequence(name = "sequence2", memory = True)
    sequence2.add_child(fallsq2)
    sequence2.add_child(turn90sq2)
    sequence2.add_child(found_gatesq2)
    
    
    risesq3 = fall_1m("risesq3")
    turn90sq3 = turn90cw("turn90sq3")
    found_gatesq3 = io_is_gate("found_gatesq3")
    
    sequence3 = Sequence(name = "sequence2", memory = True)
    sequence3.add_child(risesq3)
    sequence3.add_child(turn90sq3)
    sequence3.add_child(found_gatesq3)
    
    selector2 = Selector(name = "selector2", memory = True)
    selector2.add_child(sequence2)
    selector2.add_child(sequence3)
    
    decorator1 = Retry(name = "decorator1", child = selector2, num_failures=8)
    
    
    found_gatesl1 = io_is_gate("found_gatesl1")
     
    selector1 = Selector(name = "selector1", memory = True)
    
    selector1.add_child(found_gatesl1)    
    selector1.add_child(decorator1)
    
    
    sequence1 = Sequence(name = "sequence1", memory = True)
    
    sequence1.add_child(selector1)
    sequence1.add_child(sequence4)
    
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
