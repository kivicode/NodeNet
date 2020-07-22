from global_vars import GLOBALS
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *
from node import Node
from node_pin import *

class QNEGraphicsView(QGraphicsView):

	def __init__(self, scene, parent=False):
		super().__init__(parent)
		self.parent = parent
		self.scene = scene
		self.currentZoom = 1.
		self.zoomClamp = False

		self.initUI()
		self.setScene(self.scene)

	def initUI(self):
		self.setRenderHints(QPainter.Antialiasing | QPainter.HighQualityAntialiasing | QPainter.TextAntialiasing | QPainter.SmoothPixmapTransform)
		self.setViewportUpdateMode(QGraphicsView.FullViewportUpdate)
		self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)

	def mouseMoveEvent(self, event):
		super().mouseMoveEvent(event)
		GLOBALS.mouse_pos = self.mapToScene(event.pos())
		# GLOBALS.mouse_pos.setY(-GLOBALS.mouse_pos.y())
		if GLOBALS.mose_pressed:
			for i in range(len(self.parent.scene.connections)):
				self.parent.scene.connections[i].graphConnection.avoid_whiteList = []
		for i in range(len(self.parent.scene.connections)):
			self.parent.scene.connections[i].graphConnection.update()

	def mousePressEvent(self, event):
		if event.button() == Qt.MiddleButton:
			self.mouseWheelPressed(event)
		elif event.button() == Qt.LeftButton:
			self.mouseLeftPressed(event)
		elif event.button() == Qt.RightButton:
			self.mouseRightPressed(event)
		else:
			super().mousePressEvent(event)

	def mouseReleaseEvent(self, event):
		if event.button() == Qt.MiddleButton:
			self.mouseWheelReleased(event)
		elif event.button() == Qt.LeftButton:
			self.mouseLeftReleased(event)
		elif event.button() == Qt.RightButton:
			self.mouseRightReleased(event)
		else:
			super().mouseReleaseEvent(event)

	def mouseLeftPressed(self, event):
		super().mousePressEvent(event)
		print('asadsa')
		GLOBALS.mose_pressed = True
		for i in range(len(self.parent.scene.connections)):
			self.parent.scene.connections[i].graphConnection.updateSelection()
			# self.parent.scene.connections[i].graphConnection.avoid_whiteList = []

		for pin in self.parent.scene.pins:
			if not pin.position in [GLOBALS.RIGHT_TOP, GLOBALS.RIGHT_BOTTOM]: # only outputs are accepted
				continue

			# pin.graphPin.update()
			# if pin.graphPin.isSelected():
			# 	GLOBALS.dragged_pin = pin
			# 	GLOBALS.start_drag = True
			# 	GLOBALS.temp_connection_id = len(self.parent.scene.connections)
			# 	self.parent.spawnConnection(pin, None)

	def mouseLeftReleased(self, event):
		super().mouseReleaseEvent(event)
		GLOBALS.mose_pressed = False
		# if GLOBALS.start_drag:

		if GLOBALS.dragged_pin is not None:
			for pin in self.parent.scene.pins:
				if pin.graphPin.is_layer_output != GLOBALS.dragged_pin.graphPin.is_layer_output: continue
				bbox = pin.graphPin.boundingRect()
				
				_pos = pin.graphPin.pos() # local in-node coordiantes
				_pos += pin.node.graphNode.pos()

				bbox.setX(_pos.x()-pin_radius)
				bbox.setY(_pos.y()-pin_radius)
				bbox.setWidth(pin_radius*2)
				bbox.setHeight(pin_radius*2)

				if bbox.contains(GLOBALS.mouse_pos.x(), GLOBALS.mouse_pos.y()):
					GLOBALS.target_pin = pin

		if GLOBALS.target_pin is None and GLOBALS.dragged_pin is not None: # nothing selected as target, delete connection
			self.parent.scene.removeConnection(self.parent.scene.connections[GLOBALS.temp_connection_id])
		elif GLOBALS.dragged_pin is not None and GLOBALS.target_pin.is_input and GLOBALS.target_pin.node != GLOBALS.dragged_pin.node:
			conn = self.parent.scene.connections[GLOBALS.temp_connection_id]
			self.parent.scene.removeConnection(conn)
			conn.second_pin = GLOBALS.target_pin
			self.parent.scene.addConnection(conn)
		elif GLOBALS.dragged_pin is not None:
			self.scene.removeItem(self.parent.scene.connections[GLOBALS.temp_connection_id].graphConnection)
			del self.parent.scene.connections[GLOBALS.temp_connection_id]

		GLOBALS.temp_connection_id = -1
		GLOBALS.target_pin = None
		GLOBALS.dragged_pin = None
		GLOBALS.start_drag = False

	def mouseRightPressed(self, event):
		super().mousePressEvent(event)

	def mouseRightReleased(self, event):
		super().mouseReleaseEvent(event)

	def mouseWheelPressed(self, event):
		releaseEvent = QMouseEvent(QEvent.MouseButtonRelease, event.localPos(), event.screenPos(), 
								   Qt.LeftButton, Qt.NoButton, event.modifiers())
		super().mouseReleaseEvent(releaseEvent)
		self.setDragMode(QGraphicsView.ScrollHandDrag)
		fakeEvent = QMouseEvent(event.type(), event.localPos(), event.screenPos(), 
								Qt.LeftButton, event.buttons() | Qt.NoButton, event.modifiers())
		super().mouseReleaseEvent(fakeEvent)
		print('Wheel Pressed', event)

	def mouseWheelReleased(self, event):
		fakeEvent = QMouseEvent(event.type(), event.localPos(), event.screenPos(), 
								Qt.LeftButton, event.buttons() & -Qt.NoButton, event.modifiers())
		super().mouseReleaseEvent(fakeEvent)
		self.setDragMode(QGraphicsView.NoDrag)
		print('Wheel Released', event)

	# def wheelEvent(self, event):
	# 	zoom_out_factor = 1. / zoom_in_factor

	# 	old_pos = self.mapToScene(event.pos())

	# 	if event.angleDelta().y() > 0:
	# 		zoom_factor = zoom_in_factor 
	# 		self.currentZoom += zoom_step
	# 	else:
	# 		zoom_factor = zoom_out_factor 
	# 		self.currentZoom -= zoom_step

	# 	self.currentZoom *= zoom_factor
	# 	# if self.currentZoom > -1:
	# 		# self.scale(zoom_factor, zoom_factor)
		
	# 	new_pos = self.mapToScene(event.pos())
	# 	delta = new_pos - old_pos
	# 	self.translate(delta.x(), delta.y())
