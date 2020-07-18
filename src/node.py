from node_graph import QNEGraphicsNode
from node_pin import *
from settings import *

class Node:

	def __init__(self, scene, pos, title='Untitled node', inputs=[0,]*3, outputs = [0,]*5):
		self.scene = scene
		self.title = title
		self.pos = pos

		self.graphNode = QNEGraphicsNode(self, self.title)
		self.graphNode.setPos(self.pos)

		self.scene.addNode(self)
		self.scene.graphScene.addItem(self.graphNode)

		self.inputs = []
		self.outputs = []
		self.count = max(len(inputs), len(outputs))
		counter = 0

		for item in inputs:
			socket = Pin(node=self, index=counter, position=LEFT_BOTTOM)
			counter += 1
			self.inputs.append(socket)

		counter = 0
		for item in outputs:
			socket = Pin(node=self, index=counter, position=RIGHT_TOP)
			counter += 1
			self.outputs.append(socket)



	def getNodePosition(self, index, position):
		x = 0 if (position in (LEFT_TOP, LEFT_BOTTOM)) else node_width

		if position in (LEFT_BOTTOM, RIGHT_BOTTOM):
			# start from bottom
			y = node_title_height + self.graphNode.height - node_border_radius - node_title_vpadding + pin_up_padding + index * pin_hspacing
		else :
			# start from top
			y = node_title_height + node_title_vpadding + node_border_radius  + index * pin_spacing
		return x, y

