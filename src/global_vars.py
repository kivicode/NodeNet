from PyQt5.QtCore import QPoint

class GLOBALS:
	mouse_pos = QPoint(0, 0)
	mose_pressed = False

	dragged_pin = None
	target_pin = None
	start_drag = False
	temp_connection_id = -1

	LEFT_TOP = 1
	LEFT_BOTTOM = 2
	RIGHT_TOP = 3
	RIGHT_BOTTOM = 4