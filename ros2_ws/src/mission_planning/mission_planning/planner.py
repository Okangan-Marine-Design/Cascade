import rclpy
from rclpy.node import Node
from cascade_msgs.msg import MovementCommand
from cascade_msgs.srv import FindObject
from cascade_msgs.srv import Status as CascadeStatus 
from cascade_msgs.msg import Classes
import py_trees
from py_trees.behaviour import Behaviour
from py_trees.common import Status
from py_trees.composites import Sequence
from py_trees.composites import Selector
from py_trees.decorators import Retry
from py_trees.decorators import Repeat
from py_trees import display  
from geometry_msgs.msg import Vector3


class MovementBehaviour(Behaviour):
    def __init__(self, name, command):
        super(MovementBehaviour, self).__init__(name)
        self.node = rclpy.create_node(f'_{name}_node')
        self.publisher = self.node.create_publisher(MovementCommand, 'movement_command', 10)
        self.client = self.node.create_client(CascadeStatus, '/navigator_status')
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.node.get_logger().info(f'Service /navigator_status not available, waiting again for {name}...')
        self.message_sent = False
        self.command = command

    def setup(self):
        self.logger.debug(f"{self.name}::setup")

    def initialise(self):
        self.logger.debug(f"{self.name}::initialise")

    def update(self):
        self.logger.debug(f"  {self.name} [update()]")
        if not self.message_sent:
            self.publish_movement_command()
            self.message_sent = True

        request = CascadeStatus.Request()
        self.future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self.node, self.future)

        if self.future.result() is not None:
            response = self.future.result()
            if response.ongoing:
                return Status.RUNNING
            else:
                if response.success:
                    self.message_sent = False
                    return Status.SUCCESS
                else:
                    return Status.FAILURE
        else:
            self.logger.debug(f"  {self.name} [update()] - Service call failed")
            return Status.FAILURE

    def publish_movement_command(self):
        msg = MovementCommand()
        msg.command = self.command
        msg.header.stamp = self.node.get_clock().now().to_msg()
        self.set_command_data(msg)
        self.publisher.publish(msg)
        self.logger.debug(f"Published MovementCommand with command: {self.command}")

    def set_command_data(self, msg):
        raise NotImplementedError

    def terminate(self, new_status):
        self.logger.debug(f"  {self.name} [terminate()][{self.status}->{new_status}]")

    def shutdown(self):
        self.node.destroy_node()


class move_1m(MovementBehaviour):
    def __init__(self, name):
        super(move_1m, self).__init__(name, MovementCommand.MOVE_RELATIVE)
    
    def set_command_data(self, msg):
        msg.data0 = Vector3(x=1.0, y=0.0, z=0.0)  # Move forward by 1 meter
        msg.data1 = Vector3(x=0.0, y=0.0, z=0.0)  # No rotation


class rise_1m(MovementBehaviour):
    def __init__(self, name):
        super(rise_1m, self).__init__(name, MovementCommand.MOVE_RELATIVE)
    
    def set_command_data(self, msg):
        msg.data0 = Vector3(x=0.0, y=0.0, z=1.0)  # Rise by 1 meter
        msg.data1 = Vector3(x=0.0, y=0.0, z=0.0)  # No rotation


class fall_1m(MovementBehaviour):
    def __init__(self, name):
        super(fall_1m, self).__init__(name, MovementCommand.MOVE_RELATIVE)
    
    def set_command_data(self, msg):
        msg.data0 = Vector3(x=0.0, y=0.0, z=-1.0)  # Fall by 1 meter
        msg.data1 = Vector3(x=0.0, y=0.0, z=0.0)  # No rotation


class turn90cw(MovementBehaviour):
    def __init__(self, name):
        super(turn90cw, self).__init__(name, MovementCommand.MOVE_RELATIVE)
    
    def set_command_data(self, msg):
        msg.data0 = Vector3(x=0.0, y=0.0, z=0.0)  # No linear movement
        msg.data1 = Vector3(x=0.0, y=0.0, z=90.0)  # Turn 90 degrees clockwise


class move_to_gate(MovementBehaviour):
    def __init__(self, name):
        super(move_to_gate, self).__init__(name, MovementCommand.GO_TO_GATE)
    
    def set_command_data(self, msg):
        msg.data0 = Vector3(x=1.0, y=0.0, z=0.0)  # Example data for stopping distance

#Defining a condition
class found_gate(Behaviour):
    def __init__(self, name):
        super(found_gate, self).__init__(name)
        self.node = rclpy.create_node('_found_gate_node')  # Create a ROS2 node
        self.client = self.node.create_client(FindObject, '/find_object')
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.node.get_logger().info('Service /find_object not available, waiting again...')

    def setup(self):
        self.logger.debug(f"found_gate::setup{self.name}")

    def initialise(self):
        self.logger.debug(f"found_gate::initialise{self.name}")

    def update(self):
        self.logger.debug("  %s [found_gate::update()]" % self.name)
        
        request = FindObject.Request()
        request.object_type = Classes.GATE  # Specify the object to find

        self.future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self.node, self.future)

        if self.future.result() is not None:
            response = self.future.result()
            if response.exists:
                self.logger.debug(f"  %s [found_gate::update()] - Found gate" % self.name)
                return Status.SUCCESS
            else:
                self.logger.debug(f"  %s [found_gate::update()] - Gate not found" % self.name)
                return Status.FAILURE
        else:
            self.logger.debug(f"  %s [found_gate::update()] - Service call failed" % self.name)
            return Status.FAILURE

    def terminate(self, new_status):
        self.logger.debug("  %s found_gate::terminate().terminate()][%s->%s]" % (self.name, self.status, new_status))

    def shutdown(self):
        self.node.destroy_node()
        
#defining the creating of the behaviour tree
#here I start with the lowest nodes and build up
    
class PlannerNode(Node):
    def __init__(self):
        super().__init__ ("Automated_Planner")
    
    def publish(self, msg):
        self.publisher_.publish(msg)

    def subscription_callback(self, msg):
        pass

def main(args=None):
    rclpy.init(args=args)
    node = PlannerNode()

    root = py_trees.composites.Sequence(name = "root", memory = True)
    
    move_tosq4 = move_to_gate("move_tosq4")
    forwardsq4 = move_1m("forwardsq4")
    
    sequence4 = Sequence(name = "sequence4", memory = True)
    sequence4.add_child(move_tosq4)
    sequence4.add_child(forwardsq4)
    
    fallsq2 = fall_1m("fallsq2")
    turn90cwsq2 = turn90cw("turn90cwsq2")
    found_gatesq2 = found_gate("found_gatesq2")
    
    sequence2 = Sequence(name = "sequence2", memory = True)
    sequence2.add_child(turn90cwsq2)
    sequence2.add_child(fallsq2)
    sequence2.add_child(found_gatesq2)
    
    
    risesq3 = rise_1m("risesq3")
    turn90cwsq3 = turn90cw("turn90sqcw3")
    found_gatesq3 = found_gate("found_gatesq3")
    
    sequence3 = Sequence(name = "sequence3", memory = True)
    sequence3.add_child(risesq3)
    sequence3.add_child(turn90cwsq3)
    sequence3.add_child(found_gatesq3)
    
    selector2 = Selector(name = "selector2", memory = True)
    selector2.add_child(sequence2)
    selector2.add_child(sequence3)
    
    decorator1 = Retry(name = "decorator1", child = selector2, num_failures=8)
    
    found_gatesl1 = found_gate("found_gatesl1")
     
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
    node.get_logger().info(py_trees.display.unicode_tree(root=root))
    behaviour_tree.setup(timeout=15)

    def print_tree(tree):
        #node.get_logger().info(py_trees.display.unicode_tree(root=tree.root, show_status=True))
        pass

    while(rclpy.ok()):
        rclpy.spin_once(node, timeout_sec=0.1)
        try:
            behaviour_tree.tick_tock(
                period_ms=250,
                number_of_iterations=1,
                pre_tick_handler=print_tree,
                post_tick_handler=None
            )
        except KeyboardInterrupt:
            behaviour_tree.interrupt()
 
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
