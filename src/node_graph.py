from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *

class QNEGraphicsNode(QGraphicsItem):

	def __init__(self, node, title='NGI', parent=None):
		super().__init__(parent)
		self._node = node
		self._title = title
		self.initTitle()
		self.initUI()

	def initTitle(self):
		self.title_item = QGraphicsTextItem(self)
		self.title_item.setDefaultTextColor(node_title_color)
		self.title_item.setFont(node_title_font)
		self.title_item.setPlainText(self._title)
		self.title_item.setPos(node_title_vpadding, node_title_hpadding)
		self.title_item.setTextWidth(node_width - 2 * node_title_vpadding)

	def initUI(self):
		self.setFlag(QGraphicsItem.ItemIsSelectable)
		self.setFlag(QGraphicsItem.ItemIsMovable)

	def boundingRect(self):
		return QRectF(
			0,
			0,
			2 * node_border_radius + node_width,
			2 * node_title_color + self.height
		).normalized()

	def paint(self, painter, _, widget=None, **argv):
		
		# title
		shift = 1 if max(len(self._node.inputs), len(self._node.outputs)) == 0 else 0
		path_title = QPainterPath()
		path_title.setFillRule(Qt.WindingFill)
		path_title.addRoundedRect(0, 0, node_width, node_title_height, node_border_radius, node_border_radius)
		path_title.addRect(-shift, node_title_height - node_border_radius + shift, node_border_radius, node_border_radius)
		path_title.addRect(node_width - node_border_radius + shift, node_title_height - node_border_radius + shift, node_border_radius, node_border_radius)
		painter.setPen(Qt.NoPen)
		painter.setBrush(node_title_brush)
		painter.drawPath(path_title.simplified())

		# content
		shift = 1 if max(len(self._node.inputs), len(self._node.outputs)) == 0 else 0
		path_body = QPainterPath()
		path_body.setFillRule(Qt.WindingFill)
		path_body.addRoundedRect(0, node_title_height, node_width, self.height - node_title_height, node_border_radius, node_border_radius)
		path_body.addRect(-shift, node_title_height + shift, node_border_radius, node_border_radius)
		path_body.addRect(node_width - node_border_radius + shift, node_title_height + shift, node_border_radius, node_border_radius)
		painter.setPen(Qt.NoPen)
		painter.setBrush(node_body_brush)
		painter.drawPath(path_body.simplified())

		# inputs

		# outline
		path_outline = QPainterPath()
		path_outline.addRoundedRect(0, 0, node_width, self.height, node_border_radius, node_border_radius)
		painter.setPen(node_default_pen if not self.isSelected() else node_selected_pen)
		painter.setBrush(Qt.NoBrush)
		painter.drawPath(path_outline.simplified())

	@property
	def title(self): return self._title

	@title.setter
	def title(self, val): 
		self._title = val
		self.title_item.setPlainText(self._title)

	@property
	def height(self):
		return node_title_height + node_pin_height * max(len(self._node.inputs), len(self._node.outputs))

	