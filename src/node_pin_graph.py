from global_vars import GLOBALS
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *


class QNEGraphicsPin(QGraphicsItem):

	def __init__(self, title, pin, is_input, is_layer_output, is_dublicate, parent=None):
		super().__init__(parent)
		self._parent = parent
		self._pin = pin
		self._title = title
		self.is_input = is_input
		self.is_layer_output = is_layer_output
		self.is_dublicate = is_dublicate

		self.initUI()


	def initUI(self):
		self.setFlag(QGraphicsItem.ItemIsSelectable)
		self.setFlag(QGraphicsItem.ItemIsFocusable)
		self.initTitle()

	def initTitle(self):
		self.width = 12 * len(self._title)
		if self.is_input:
			title_x = node_title_vpadding + node_title_xshift
		else:
			title_x = node_title_vpadding + node_title_xshift - node_width

		if not self.is_dublicate:
			self.edit_item = QEditableTextItem(self)
			if not self.is_layer_output and self.is_input:
				self.edit_item.setFlag(QGraphicsItem.ItemIsSelectable)
				self.edit_item.setFlag(QGraphicsItem.ItemIsFocusable)
				self.edit_item.setDefaultTextColor(node_title_font_color)
			else:
				self.edit_item.setDefaultTextColor(pin_output_font_color)
			self.edit_item.setZValue(100)
			self.edit_item.setFont(node_title_font)
			self.edit_item.setPlainText(self._title)
			self.edit_item.setPos(40 + title_x, node_title_hpadding - node_title_font.pointSize()*1.5)
			self.edit_item.setTextWidth(node_width - 3 * node_title_vpadding - 40)

		self.title_item = QGraphicsTextItem(self)
		self.title_item.setDefaultTextColor(node_title_font_color)
		self.title_item.setFont(node_title_font)
		self.title_item.setPlainText(self._title)
		self.title_item.setPos(title_x, node_title_hpadding - node_title_font.pointSize()*1.5)
		self.title_item.setTextWidth(node_width - 2 * node_title_vpadding)


	def paint(self, painter, _, widget=None):
		painter.setBrush(pin_layer_brush if self.is_layer_output else pin_brush)
		painter.setPen(node_default_pen)
		painter.drawEllipse(-pin_radius, -pin_radius, 2 * pin_radius, 2 * pin_radius)

	def boundingRect(self):
		return QRectF(
			- pin_radius,
			- pin_radius,
			2 * pin_radius,
			2 * pin_radius,
		)

	def mousePressEvent(self, event):
		super().mousePressEvent(event)
		if self.isSelected() and self._pin.position in [GLOBALS.RIGHT_BOTTOM, GLOBALS.RIGHT_TOP]:
			GLOBALS.dragged_pin = self._pin
			GLOBALS.start_drag = True
			GLOBALS.temp_connection_id = len(GLOBALS.main_window.scene.connections)
			GLOBALS.main_window.spawnConnection(self._pin, None)



class QEditableTextItem(QGraphicsTextItem):

	def __init__(self, parent=None, *args, **argv):
		super().__init__(parent, *args, **argv)


	def focusInEvent(self, event):
		self.setTextInteractionFlags(Qt.TextEditable)

	def boundingRect(self):
		return QRectF(0, 0, self.textWidth(), int(node_title_font.pointSize() * 1.5 + 2))

	def focusOutEvent(self, event):
		self.setTextInteractionFlags(Qt.NoTextInteraction)
		_cursor = self.textCursor();
		_cursor.clearSelection();
		self.setTextCursor(_cursor);
		self.update()
		self.setTextInteractionFlags(Qt.TextEditable)

	def paint(self, painter, _, widget=None):
		painter.setBrush(pin_value_background)
		painter.setPen(Qt.NoPen)

		h = node_title_font.pointSize()
		painter.drawRect(0, h // 10, self.boundingRect().width(), self.boundingRect().height())
		super().paint(painter, _, widget)
