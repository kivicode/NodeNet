from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *


class QNEGraphicsPin(QGraphicsItem):

	def __init__(self, title, parent=None):
		super().__init__(parent)
		self._parent = parent
		self._title = title
		print(title)

		self.initUI()


	def initUI(self):
		self.setFlag(QGraphicsItem.ItemIsSelectable)
		self.setFlag(QGraphicsItem.ItemIsFocusable)
		self.initTitle()

	def initTitle(self):
		self.title_item = QGraphicsTextItem(self)
		self.title_item.setDefaultTextColor(node_title_font_color)
		self.title_item.setFont(node_title_font)
		self.title_item.setPlainText(self._title)
		self.title_item.setPos(node_title_vpadding + node_title_xshift, node_title_hpadding - node_title_font.pointSize()*1.5)
		self.title_item.setTextWidth(node_width - 2 * node_title_vpadding)

		self.edit_item = QEditableTextItem(self)
		self.edit_item.setFlag(QGraphicsItem.ItemIsSelectable)
		self.edit_item.setFlag(QGraphicsItem.ItemIsFocusable)
		self.edit_item.setDefaultTextColor(node_title_font_color)
		self.edit_item.setFont(node_title_font)
		self.edit_item.setPlainText(self._title)
		self.edit_item.setPos(40 + node_title_vpadding + node_title_xshift, node_title_hpadding - node_title_font.pointSize()*1.5)
		self.edit_item.setTextWidth(node_width - 3 * node_title_vpadding - 40)


	def paint(self, painter, _, widget=None):
		painter.setBrush(pin_brush)
		painter.setPen(node_default_pen)
		painter.drawEllipse(-pin_radius, -pin_radius, 2 * pin_radius, 2 * pin_radius)

	def boundingRect(self):
		return QRectF(
			- pin_radius,
			- pin_radius,
			2 * pin_radius,
			2 * pin_radius,
		)

	def focusInEvent(self, event):
		print('in')

	def focusOutEvent(self, event):
		print('out')



class QEditableTextItem(QGraphicsTextItem):

	def __init__(self, parent=None, *args, **argv):
		super().__init__(parent, *args, **argv)
		# self.setTextInteractionFlags(Qt.NoTextInteraction)
		self.setTextInteractionFlags(Qt.TextEditable)

	def focusInEvent(self, event):
		pass
		self.setHtml("<div style='background-color:#ff0000;'>" + 'yourText' + "</div>");
		# self.setTextInteractionFlags(Qt.TextEditable)

	def focusOutEvent(self, event):
		pass
		_cursor = self.textCursor()
		_cursor.clearSelection()
		self.setTextCursor(_cursor);

	# def paint(self, painter, _, widget=None):
	# 	painter.setBrush(pin_value_background)
	# 	painter.setPen(Qt.NoPen)

	# 	h = node_title_font.pointSize()
	# 	painter.drawRect(0, (-h * 1.25) / 2, self.textWidth(), h * 1.25)
