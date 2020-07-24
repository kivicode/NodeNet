from node_pin_graph import QNEGraphicsPin
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *
from global_vars import GLOBALS


class Pin():
	def __init__(self, node, connections=[], title='ab', index=0, position=GLOBALS.LEFT_TOP, is_layer_output=False, is_dublicate=False, value=None, dtype='const'):

		self.node = node
		self.index = index
		self.position = position
		self.is_input = position in [GLOBALS.LEFT_BOTTOM, GLOBALS.LEFT_TOP]
		self.title = title
		self.value = value
		self.dtype = dtype

		self.graphPin = QNEGraphicsPin(title, self, self.is_input, \
													is_layer_output, \
													is_dublicate, \
													parent=self.node.graphNode)

		self.updatePos()
		self.connections = connections

		self.node.scene.addPin(self)

	def updatePos(self):
		newPos = self.node.getNodePosition(self.index, self.position)
		self.graphPin.setPos(*newPos)

	def setValue(self, val):
		self.value = val
		self.graphPin.edit_item.setPlainText(str(val))