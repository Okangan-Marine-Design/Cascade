"""
Notes about the current version of automated planning

1. For this version we are commiting to the blue path. If there is an issue with this please tell 
Raph or someone else on the software team.

2. This will only be trying for the gate and buoy task as discussed before. If there are any changes please tell Raph.

3. Currently this build assumes that we are not able to use the paths provided by the competition.
I have implemented a search pattern to go from the gate to the buoy to the finish line. If we
become able to recognize the paths before the competition please tell Raph.

4. This build will try for style points AFTER going through the gate. This means 8 full rotations.
If we need to change this please tell Raph.

"""











import py_trees
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees.composites import Selector
from py_trees.decorators import Retry
from py_trees.decorators import Repeat
from py_trees import display  


#Defining the action behaviours for locomotion

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
        

#This following are action behaviours for moving to a significant object
class move_to_gate(Behaviour): 
    def __init__(self,name):
        super(move_to_gate,self).__init__(name)
        
    def setup(self):
        self.logger.debug(f"move_to_gate::setup{self.name}")
        
    def initialise(self):
        self.logger.debug(f"move_to_gate::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [move_to_gate::update()]" % self.name)
        print("move_to_gate")
        return Status.SUCCESS
    
    def terminate(self, new_status):
        self.logger.debug("  %s [move_to_gate::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))
        
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
        

        

        

        
        
#the following are conditions for checking for the presence of an important object
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
        
        
#This is where the bahaviour tree is created
if __name__ == '__main__': 
    root = py_trees.composites.Sequence(name = "root", memory = True)