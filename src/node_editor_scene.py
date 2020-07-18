from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *
from math import floor, ceil

class QNEGraphicsScene(QGraphicsScene):

	def __init__(self, scene, parent=None):
		super().__init__(parent)

		self.scene = scene
		
		self._background_color = QColor(*(background_color,) * 3)
		self._grid_color = QColor(*(grid_color,) * 3)

		self._grid_pen = QPen(self._grid_color)
		self._grid_pen.setWidth(1)

		self.width, self.height = 128000, 64000

		self.setBackgroundBrush(self._background_color)

	def setGraphScene(self, width, height):
		self.setSceneRect(-width // 2, -height // 2, width, height)	


	def drawBackground(self, painter, rect):
		super().drawBackground(painter, rect)

		top, bottom = int(floor(rect.top())), int(ceil(rect.bottom()))
		left, right = int(floor(rect.left())), int(ceil(rect.right()))

		first_top = top - (top % grid_size)
		first_left = left - (left % grid_size)

		# calculate grid
		grid_dots = []
		for x in range(first_left, right, grid_size):
			for y in range(first_top, bottom, grid_size):
				grid_dots.append(QRect(x, y, 1, 1))


		# draw grid
		painter.setPen(self._grid_pen)
		painter.drawRects(*grid_dots)
		