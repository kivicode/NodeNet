from node_graph import QNEGraphicsNode
from global_vars import GLOBALS
from node_pin import *
from settings import *
from io_pins import *
from interpreter import Interpreter

class Node:

	def __init__(self, scene, pos, title='Untitled node', inputs=[Input('a', 'const'), Input('b', 'const'), Input('d', 'const')], outputs=[Output('c', 'const'), Output('e', 'const')], connections=[]):
		self.scene = scene
		self.title = title
		self.connections = []
		self.inputs = []
		self.outputs = []

		self.graphNode = QNEGraphicsNode(self, self.title)
		self.setPos(pos)

		self.scene.addNode(self)
		self.scene.graphScene.addItem(self.graphNode)

		self.setIOPins(inputs, outputs)

	def setPos(self, pos):
		self.position = pos
		self.graphNode.setPos(self.position)

	@property
	def pos(self):
		return self.graphNode.pos()

	def getNodePosition(self, index:int, position:int, num_out_of:int=1) -> '(x, y)':
		x = 0 if (position in (GLOBALS.LEFT_TOP, GLOBALS.LEFT_BOTTOM)) else self.graphNode.width

		if position in (GLOBALS.LEFT_BOTTOM, GLOBALS.RIGHT_BOTTOM):
			# start from bottom
			y = self.graphNode.height - node_border_radius - node_title_vpadding - index * pin_hspacing
		elif position in (GLOBALS.LEFT_TOP, GLOBALS.RIGHT_TOP):
			# start from top
			y = node_title_height + node_title_vpadding + node_border_radius + index * pin_spacing
		else:
			# this should never happen
			y = 0

		return int(x), int(y)

	def setIOPins(self, inputs, outputs):
		for pin in self.inputs + self.outputs:
			self.scene.graphScene.removeItem(pin.graphPin)

		self.inputs = []
		self.outputs = []
		# self.count = max(len(inputs), len(inputs))
		self.inputs_counter = 0
		self.outputs_counter = 0

		for item in inputs:
			is_layer = item.dtype == 'layer'
			self.addPin(item.title, True, is_layer, False)
			self.addPin('', False, is_layer, True)

		for item in outputs:
			self.addPin(item.title, False, item.dtype == 'layer')

	def addPin(self, title, is_input, is_layer_output=False, is_dublicate=False):
		pin = Pin(node=self, index=self.inputs_counter if is_input else self.outputs_counter, \
							 title=title, \
							 position=GLOBALS.LEFT_TOP if is_input else GLOBALS.RIGHT_TOP, \
							 is_layer_output=is_layer_output, \
							 is_dublicate=is_dublicate)
		if is_input:
			self.inputs_counter += 1
			self.inputs.append(pin)
		else:
			self.outputs_counter += 1
			self.outputs.append(pin)
		for io in self.inputs + self.outputs:
			io.graphPin.initSizes()
		for io in self.inputs + self.outputs:
			io.graphPin.initTitle()
	# def removeInput(self, idx):
	# 	_input = self.inputs[idx]
	# 	dublicate_output = self.outputs[idx]
	# 	for i in range(idx, len(self.inputs)):
	# 		self.inputs[i].index += 1
	# 		self.inputs[i].updatePos()

	# 	for i in range(idx, len(self.outputs)):
	# 		self.outputs[i].index -= 1
	# 		self.outputs[i].updatePos()

	# 	self.scene.graphScene.removeItem(_input.graphPin)
	# 	self.scene.graphScene.removeItem(self.outputs[idx].graphPin) # remove dublicate
	# 	self.inputs.remove(_input)
	# 	self.outputs.remove(self.outputs[idx])


	def buildFromFile(self, filename):
		with open(filename, 'r') as f:
			code = f.read()
			self.buildFromCode(code)

	def buildFromCode(self, code):
		self.setIOPins([], []) # clear pins
		inputs_dict  = Interpreter.extractInputs(code, target_parent=self)
		outputs_dict = Interpreter.extractOutputs(code, target_parent=self)
		for input_pin in inputs_dict.values():
			input_pin.build()

		for output_pin in outputs_dict.values():
			output_pin.build()


