from node_editor_scene import QNEGraphicsScene

class Scene:

	def __init__(self):
		self.nodes = []
		self.edges = []

		self.width = 128000
		self.height = 64000

		self.initUI()

	def initUI(self):
		self.graphScene = QNEGraphicsScene(self)
		self.graphScene.setGraphScene(self.width, self.height)

	def addNode(self, node):
		self.nodes.append(node)

	def addEdge(self, edge):
		self.edges.append(edges)

	def removeNode(self, node):
		self.nodes.remove(node)

	def removeEdge(self, edge):
		self.edges.remove(edges)