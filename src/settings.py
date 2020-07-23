from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont, QPen, QBrush, QColor, QPalette

background_color = 21

grid_color = 47
grid_width = 2
grid_size = 20

node_width = 250
node_title_height = 25
node_pin_height = 30

zoom_in_factor = 1.25
zoom_step = .5
zoom_range = [0, 10]

node_default_pen = QPen(QColor(80, 80, 80))
node_default_pen.setWidth(2)

node_selected_pen = QPen(QColor(208, 208, 208))
node_selected_pen.setWidth(3.5)

node_title_color = Qt.white
node_title_font = QFont('Helvetica', 12, QFont.Medium)
node_title_font_color = QColor(200, 200, 200)
node_title_brush = QBrush(QColor(80, 80, 80))
node_title_vpadding = 10
node_title_hpadding = 5
node_title_xshift = 10

node_body_brush = QBrush(QColor(32, 32, 32))

node_title_height = 30
node_pin_height = 25
node_border_radius = 10.

pin_brush = QBrush(QColor(244, 191, 117))
pin_layer_brush = QBrush(QColor(201, 236, 124))
pin_radius = 6.5
pin_hspacing = -3
pin_spacing  = 22
pin_up_padding = 5
pin_editor_margin = 20
pin_value_background = QBrush(QColor(48, 48, 48))
pin_output_font_color = QColor(90, 90, 90)

connection_pen = QPen(QColor(244, 191, 117))
connection_pen.setWidth(4)
connection_layer_pen = QPen(QColor(201, 236, 124))
connection_layer_pen.setWidth(4)
connection_selected_pen = QPen(QColor(255, 255, 163))
connection_selected_pen.setWidth(4)
connection_shift_ratio = 6
connection_shift = 10

wire_skip_size = 15
draw_skips = False