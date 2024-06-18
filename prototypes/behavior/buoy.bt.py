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
class move_to_buoy(Behaviour): 
    def __init__(self,name):
        super(move_to_buoy,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"move_to_buoy::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"move_to_buoy::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [move_to_buoy::update()]" % self.name)
        print("move_to_buoy")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [move_to_buoy::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
           
        
#Defining the conditions
class io_is_buoy(Behaviour):
    def __init__(self,name):
        super(io_is_buoy,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"io_is_buoy::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"io_is_buoy::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [io_is_buoy::update()]" % self.name)
        if 1 == 1:     #replace later with if io(identified object) == "buoy"
            return Status.SUCCESS
        else:
            return Status.FAILURE
    
    def terminate(self, new_status):
        self.logger.debug("  %s [io_is_buoy::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
        
if __name__ == '__main__':

    root = py_trees.composites.Sequence(name = "root", memory = True)
    
    forwardsq12 = move_1m("forwardsq12")
    turn90ccwsq12 = turn90ccw("turn90ccwsq12")
    
    sequence12 = Sequence(name = "sequence12", memory = True)
    sequence12.add_child(forwardsq12)
    sequence12.add_child(turn90ccwsq12)
    
    decorator5 = Repeat(name = "decorator5", child = sequence12, num_success = 4)
    
    move_tosq11 = move_to_buoy("move_to_sq11")
    turn45cwsq11 = turn45cw("turn45cwsq11")
    
    sequence11 = Sequence(name = "sequence11", memory = True)
    sequence11.add_child(move_tosq11)
    sequence11.add_child(turn45cwsq11)
    sequence11.add_child(decorator5)
    
    turn45ccwsq10 = turn45ccw("turn45ccwsq10")
    found_polesq10 = io_is_buoy("found_polesq10")
    
    sequence10 = Sequence(name = "sequence10", memory = True)
    sequence10.add_child(turn45ccwsq10)
    sequence10.add_child(found_polesq10)
    
    found_buoysl4 = io_is_buoy("found_polesl4")
    
    selector4 = Selector(name = "selector4", memory = True)
    selector4.add_child(found_buoysl4)
    selector4.add_child(sequence10)
    
    decorator4= Retry(name = "decorator4", num_failures=8, child = selector4)
    
    sequence9 = Sequence(name = "sequence9", memory = True)
    sequence9.add_child(decorator4)
    sequence9.add_child(sequence11)
    
    root.add_child(sequence9)
    
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
