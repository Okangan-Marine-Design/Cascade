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
        

        
class turn45cw(Behaviour): 
    def __init__(self,name):
        super(turn45cw,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"turn45cw::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"turn45cw::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [turn45cw::update()]" % self.name)
        print("command turn 45 clockwise")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [turn45cw::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
class turn45ccw(Behaviour): 
    def __init__(self,name):
        super(turn45ccw,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"turn45ccw::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"turn45ccw::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [turn45ccw::update()]" % self.name)
        print("command turn 45 counter-clockwise")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [turn45ccw::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
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
        
class turn90ccw(Behaviour): 
    def __init__(self,name):
        super(turn90ccw,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"turn90ccw::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"turn90ccw::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [turn90ccw::update()]" % self.name)
        print("command turn 90 counter-clockwise")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [turn90ccw::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        

#This behavior is for moving to the currently identified object
class move_to_pole(Behaviour): 
    def __init__(self,name):
        super(move_to_pole,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"move_to_pole::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"move_to_pole::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [move_to_pole::update()]" % self.name)
        print("move_to_pole")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [move_to_pole::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
           
        
#Defining a condition
class io_is_pole(Behaviour):
    def __init__(self,name):
        super(io_is_pole,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"io_is_pole::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"io_is_pole::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [io_is_pole::update()]" % self.name)
        if 1 == 1:     #replace later with if io(identified object) == "pole"
            return Status.SUCCESS
        else:
            return Status.FAILURE
    
    def terminate(self, new_status):
        self.logger.debug("  %s [io_is_pole::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
        
if __name__ == '__main__':

    root = py_trees.composites.Sequence(name = "root", memory = True)
    
    forwardsq8 = move_1m("forwardsq8")
    turn90ccwsq8 = turn90ccw("turn90ccwsq8")
    
    sequence8 = Sequence(name = "sequence8", memory = True)
    sequence8.add_child(forwardsq8)
    sequence8.add_child(turn90ccwsq8)
    
    decorator3 = Repeat(name = "decorator3", child = sequence8, num_success = 4)
    
    move_tosq7 = move_to_pole("move_to_sq7")
    turn45cwsq7 = turn45cw("turn45cwsq7")
    
    sequence7 = Sequence(name = "sequence7", memory = True)
    sequence7.add_child(move_tosq7)
    sequence7.add_child(turn45cwsq7)
    sequence7.add_child(decorator3)
    
    turn45ccwsq6 = turn45ccw("turn45ccwsq6")
    found_polesq6 = io_is_pole("found_polesq6")
    
    sequence6 = Sequence(name = "sequence6", memory = True)
    sequence6.add_child(turn45ccwsq6)
    sequence6.add_child(found_polesq6)
    
    found_polesl3 = io_is_pole("found_polesl3")
    
    selector3 = Selector(name = "selector3", memory = True)
    selector3.add_child(found_polesl3)
    selector3.add_child(sequence6)
    
    decorator2= Retry(name = "decorator2", num_failures=8, child = selector3)
    
    sequence5 = Sequence(name = "sequence5", memory = True)
    sequence5.add_child(decorator2)
    sequence5.add_child(sequence7)
    
    root.add_child(sequence5)
    
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
