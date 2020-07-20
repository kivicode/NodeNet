from node_editor_scene import QNEGraphicsScene

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
		self.connections.append(edge)

	def addPin(self, pin):
		self.pins.append(pin)

	def removeNode(self, node):
		self.nodes.remove(node)

	def removeConnection(self, edge):
		self.connections.remove(edge)

	def removePin(self, pin):
		self.pins.remove(pin)