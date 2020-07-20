from node_graph import QNEGraphicsNode
from node_pin import *
from settings import *


class Node:

	def __init__(self, scene, pos, title='Untitled node', inputs=['a', 'b'], outputs=['']):
		self.scene = scene
		self.title = title

		self.graphNode = QNEGraphicsNode(self, self.title)
		self.setPos(pos)

		self.scene.addNode(self)
		self.scene.graphScene.addItem(self.graphNode)

		self.inputs = []
		self.outputs = []
		self.count = max(len(inputs), len(outputs))
		counter = 0

		for item in inputs:
			pin = Pin(node=self, index=counter, title=item, position=LEFT_BOTTOM)
			counter += 1
			self.inputs.append(pin)

		counter = 0
		for item in outputs:
			pin = Pin(node=self, index=counter, title='', position=RIGHT_TOP)
			counter += 1
			self.outputs.append(pin)

	def setPos(self, pos):
		self.position = pos
		self.graphNode.setPos(self.position)

	@property
	def pos(self):
		return self.graphNode.pos()

	def getNodePosition(self, index, position):
		x = 0 if (position in (LEFT_TOP, LEFT_BOTTOM)) else node_width

		if position in (LEFT_BOTTOM, RIGHT_BOTTOM):
			# start from bottom
			y = node_title_height + self.graphNode.height - node_border_radius - node_title_vpadding + pin_up_padding + index * pin_hspacing
		else:
			# start from top
			y = node_title_height + node_title_vpadding + node_border_radius + index * pin_spacing

		if position in (RIGHT_BOTTOM, RIGHT_TOP):
			y -= pin_radius - 1
		return x, y

	def fromCode(self, code):
		self.inputs = []
		self.outputs = []
