from node_connection_graph import QNEGraphicsConnection
import inspect

class Connection:

	def __init__(self, scene, first_pin=None, second_pin=None):
		self.scene = scene

		self.first_pin = first_pin
		self.second_pin = second_pin

		self.graphConnection = QNEGraphicsConnection(self)
		self.scene.addConnection(self)

	def delete(self):
		if self.first_pin is not None:
			self.first_pin.connection.remove(self)

	def __repr__(self):
		return f'<Connection first={self.first_pin}, second={self.second_pin}>'
