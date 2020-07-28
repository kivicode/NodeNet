from node_editor_scene import QNEGraphicsScene
from global_vars import *
import inspect

class Scene:

	def __init__(self):
		self.nodes = []
		self.pins = []
		self.connections = []

		self.width = 128000
		self.height = 64000

		self.initUI()

	def initUI(self):
		self.graphScene = QNEGraphicsScene(self)
		self.graphScene.setGraphScene(self.width, self.height)

	def addNode(self, node):
		self.nodes.append(node)

	def addConnection(self, edge):
		edge.first_pin.connections.append(edge)
		edge.first_pin.node.eval()
		if edge.second_pin is not None:
			edge.second_pin.connections.append(edge)
			edge.second_pin.setValue(edge.first_pin.value)
			edge.second_pin.node.eval()
		self.connections.append(edge)
		self.graphScene.addItem(edge.graphConnection)

	def addPin(self, pin):
		self.pins.append(pin)

	def removeNode(self, node):
		self.nodes.remove(node)

	def removeConnection(self, edge):
		edge.first_pin.connections.remove(edge)
		if edge.second_pin is not None:
			edge.second_pin.connections.remove(edge)
		self.connections.remove(edge)
		self.graphScene.removeItem(edge.graphConnection)

	def removePin(self, pin):
		self.pins.remove(pin)
		self.graphScene.removeItem(pin.graphPin)