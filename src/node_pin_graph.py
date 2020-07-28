from global_vars import GLOBALS, KEYS
from interpreter import Interpreter
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
		self.title_item = None
		self.edit_item = None
		self.gNode = self._pin.node.graphNode
		self.title_x = 0
		self.initSizes()
		self.initUI()


	def initUI(self):
		self.setFlag(QGraphicsItem.ItemIsSelectable)
		self.setFlag(QGraphicsItem.ItemIsFocusable)
		self.initTitle()

	def initSizes(self):
		fm = QFontMetrics(node_title_font)

		# print(, 12 * len(self._title))
		self.width = fm.width(self._title) + node_width + pin_editor_margin
		self.gNode.width = max(self.gNode.width, self.width)
		self.gNode._pin_x = max(self.gNode._pin_x, self.width - node_width)
		# self.initTitle()

	def initTitle(self):
		if self.is_input:
			self.title_x = node_title_vpadding + node_title_xshift
		else:
			self.title_x = node_title_vpadding + node_title_xshift - self.width

		if not self.is_dublicate:
			if self.edit_item is None:
				self.edit_item = QEditableTextItem(self)
			if not self.is_layer_output and self.is_input:
				self.edit_item.setFlag(QGraphicsItem.ItemIsSelectable)
				self.edit_item.setFlag(QGraphicsItem.ItemIsFocusable)
				self.edit_item.setDefaultTextColor(node_title_font_color)
			else:
				self.edit_item.setDefaultTextColor(pin_output_font_color)
			self.edit_item.setZValue(-1)
			self.edit_item.setFont(node_title_font)
			self.edit_item.setPlainText(str(self._pin.value))
			x = self.gNode._pin_x + self.title_x
			width = self.gNode.width - self.gNode._pin_x - pin_editor_margin * 1.6
			if not self.is_input:
				x = -width - round(pin_editor_margin / 1.66)
			self.edit_item.setPos(x, node_title_hpadding - node_title_font.pointSize()*1.5)
			self.edit_item.setTextWidth(width)

		if self.title_item is None:
			self.title_item = QGraphicsTextItem(self)
		self.title_item.setZValue(2)
		self.title_item.setDefaultTextColor(node_title_font_color)
		self.title_item.setFont(node_title_font)
		self.title_item.setPlainText(self._title)
		self.title_item.setPos(self.title_x, node_title_hpadding - node_title_font.pointSize()*1.5)
		self.title_item.setTextWidth(self.width - 2 * node_title_vpadding)


	def paint(self, painter, style, widget=None):
		# widget.setZValue(1000)
		painter.setBrush(pin_layer_brush if self.is_layer_output else pin_brush)
		painter.setPen(node_default_pen)
		painter.drawEllipse(self._getX(), -pin_radius, 2 * pin_radius, 2 * pin_radius)
		if self.edit_item is not None:
			self.edit_item.updateEditability()

	def boundingRect(self):
		return QRectF(
			self._getX(),
			- pin_radius,
			2 * pin_radius,
			2 * pin_radius,
		)

	def _getX(self):
		x = self.gNode.width-self.pos().x()-pin_radius
		if self.is_input:
			x = -pin_radius
		return x

	def mousePressEvent(self, event):
		super().mousePressEvent(event)
		if self.isSelected() and self._pin.position in [GLOBALS.RIGHT_BOTTOM, GLOBALS.RIGHT_TOP]:
			GLOBALS.dragged_pin = self._pin
			GLOBALS.start_drag = True
			GLOBALS.temp_connection_id = len(GLOBALS.main_window.scene.connections)
			GLOBALS.main_window.spawnConnection(self._pin, None)



class QEditableTextItem(QGraphicsTextItem):

	def __init__(self, parent=None, *args, **argv):
		self.graphPin = parent
		super().__init__(parent, *args, **argv)


	def focusInEvent(self, event):
		GLOBALS.textIsBeingEdited = True  # oh, yeah! I love long var names
		self.setTextInteractionFlags(Qt.TextEditable)

	def boundingRect(self):
		return QRectF(0, 0, self.textWidth(), int(node_title_font.pointSize() * 1.5 + 2))

	def updateEditability(self):
		return
		isEditable = len(self.graphPin._pin.connections) > 0 or not(not self.graphPin.is_layer_output and self.graphPin.is_input)
		if isEditable:
			self.setTextInteractionFlags(Qt.NoTextInteraction)
			self.setDefaultTextColor(pin_output_font_color)
		else:
			self.setTextInteractionFlags(Qt.TextEditable)
			self.setDefaultTextColor(node_title_font_color)

	def focusOutEvent(self, event):
		GLOBALS.textIsBeingEdited = False
		self.setTextInteractionFlags(Qt.NoTextInteraction)
		_cursor = self.textCursor();
		_cursor.clearSelection();
		self.setTextCursor(_cursor);
		self.update()
		self.setTextInteractionFlags(Qt.TextEditable)
		self.update()

	def keyPressEvent(self, event):
		if Interpreter.isNumber(event.text()):
			super().keyPressEvent(event)
		elif event.key() in [KEYS.ARR_LEFT, KEYS.ARR_RIGHT, KEYS.DELETE, KEYS.BACKSPACE, KEYS.SPACE]:
			super().keyPressEvent(event)
			if event.key() in [KEYS.ARR_LEFT, KEYS.ARR_RIGHT]:
				_cursor = self.textCursor();
				pos = min(len(self.toPlainText()), _cursor.position() + (1 if event.key() == KEYS.ARR_RIGHT else -1))
				_cursor.setPosition(pos);
				self.setTextCursor(_cursor);

	# def keyReleaseEvent(self, event):
	# 	super().keyReleaseEvent(event)
	# 	if Interpreter.isNumber(event.text()):
	# 		self.graphPin._pin.setValue(int(self.toPlainText() + event.text()))
	# 		self.graphPin._pin.node.eval()

	def paint(self, painter, _, widget=None):
		painter.setBrush(pin_value_background)
		painter.setPen(Qt.NoPen)

		h = node_title_font.pointSize()
		painter.drawRect(0, h // 10, self.boundingRect().width(), self.boundingRect().height())
		super().paint(painter, _, widget)
