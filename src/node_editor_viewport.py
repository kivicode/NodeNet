from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *
from node import Node

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
		if event == Qt.MiddleButton:
			self.mouseWheelPrReleased(event)
		elif event == Qt.LeftButton:
			self.mouseLeftReleased(event)
		elif event == Qt.RightButton:
			self.mouseRightReleased(event)
		else:
			super().mouseReleaseEvent(event)

	def mouseLeftPressed(self, event):
		super().mousePressEvent(event)

	def mouseLeftReleased(self, event):
		super().mouseReleaseEvent(event)

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
