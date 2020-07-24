from PyQt5.QtCore import QPoint

class GLOBALS:
	mouse_pos = QPoint(0, 0)
	mose_pressed = False

	dragged_pin = None
	target_pin = None
	start_drag = False
	temp_connection_id = -1
	textIsBeingEdited = False

	LEFT_TOP = 1
	LEFT_BOTTOM = 2
	RIGHT_TOP = 3
	RIGHT_BOTTOM = 4

class KEYS:

	SPACE = 32
	ENTER = 16777220
	DELETE = 16777223
	BACKSPACE = 16777219

	ARR_UP = 16777235
	ARR_DOWN = 16777237
	ARR_LEFT = 16777234
	ARR_RIGHT = 16777236