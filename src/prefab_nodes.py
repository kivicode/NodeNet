from node import Node

class ConstantNode(Node):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		self.buildFromFile('prefabs/constant.node')

class SquareNode(Node):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		self.buildFromFile('prefabs/square.node')