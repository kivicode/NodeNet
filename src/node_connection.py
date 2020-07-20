from node_connection_graph import QNEGraphicsConnection

class Connection:

	def __init__(self, scene, first_pin=None, second_pin=None):
		self.scene = scene

		self.first_pin = first_pin
		self.second_pin = second_pin

		self.graphConnection = QNEGraphicsConnection(self)

		self.scene.graphScene.addItem(self.graphConnection)
		self.scene.addConnection(self)

	def delete(self):
		if self.first_pin is None:
			self.first_pin.connection.remove(self)

