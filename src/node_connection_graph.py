from global_vars import GLOBALS
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *
from utils import *
import numpy as np

class QNEGraphicsConnection(QGraphicsItem):

	def __init__(self, connection, parent=None):
		super().__init__(parent)

		self.connection = connection
		self.__mouse_pos = QPoint(0,0)
		self._path = QPainterPath()
		self._poly_path = QPolygonF()
		self._hull = QPolygonF()
		self.avoid_whiteList = []
		self.true_segement = []
		self.initUI()

	def initUI(self):
		self.setFlag(QGraphicsItem.ItemIsSelectable)


	def paint(self, painter, _, widget=None, **argv):

		painter.setPen(connection_pen if not self.isSelected() else connection_selected_pen)
		painter.setBrush(Qt.NoBrush)

		painter.drawPath(self.path())
		# self.path()
		# painter.drawPolygon(self._hull)

	def updateSelection(self):
		if self._hull.containsPoint(GLOBALS.mouse_pos, Qt.WindingFill):
			self.setSelected(not self.isSelected())

	def boundingRect(self):
		return QRectF(0,0,0,0)

	def path(self):
		start_pin_id = self.connection.first_pin.index
		start_pin_alignment = self.connection.first_pin.position
		start_pos = QPoint(*self.connection.first_pin.node.getNodePosition(start_pin_id, \
		                                                                   start_pin_alignment)) # local in-node coordiantes
		start_pos += self.connection.first_pin.node.pos # global coordinates mapping
		start_pos += QPoint(pin_radius + (2 if self.isSelected() else 0), 0) # true alignment

		if self.connection.second_pin is not None:
			end_pos = self.connection.second_pin.graphPin.pos() # local in-node coordiantes
			end_pos += self.connection.second_pin.node.graphNode.pos() # global coordinates mapping

			# _pos = pin.graphPin.pos() # local in-node coordiantes
			# _pos += pin.node.graphNode.pos()

			# end_pos -= QPoint(pin_radius + (2 if self.isSelected() else 0), pin_up_padding) # true alignment
		else:
			end_pos = GLOBALS.mouse_pos

		self._path = QPainterPath(start_pos)

		delta = end_pos - start_pos
		shift = 30

		points = [QPoint(start_pos.x() + shift, start_pos.y()), \
		          QPoint(end_pos.x() - shift, end_pos.y()),
		          end_pos
		]

		if draw_skips:
			self.true_segement = points[:2]

			intersections = []
			intersections_segs = []

			for conn in self.connection.scene.connections:
				connGr = conn.graphConnection
				if connGr in self.avoid_whiteList: continue
				if self in connGr.avoid_whiteList: continue
				if conn == self: continue
				if connGr.true_segement == []: continue

				intersection = lineIntersection(self.true_segement, connGr.true_segement)
				if intersection is None: continue
				if isPointOnSegment(*connGr.true_segement, intersection) and isPointOnSegment(*self.true_segement, intersection): 
					intersections.append(intersection)
					intersections_segs.append(connGr.true_segement)
				conn.graphConnection.avoid_whiteList.append(self)

		last_point = start_pos
		for i, p in enumerate(points):

			was_intersected = False

			if draw_skips:
				nearest = p
				if intersections != []:
					nearest_intersection = minDist(last_point, intersections)
					prev_cur_dst = round(dist(last_point, p))
					prev_int_dst = round(dist(last_point, nearest_intersection))

					if prev_int_dst < prev_cur_dst:
						nearest = nearest_intersection
						idx = intersections.index(nearest)
						segment = intersections_segs[idx]
						self._path.moveTo(last_point)
						self._drawHalfCircle(self.true_segement, segment, nearest)
						self._path.lineTo(p)
						was_intersected = True

			if not was_intersected:
				self._path.moveTo(last_point)
				self._path.lineTo(p)
			last_point = p

		self._path.lineTo(end_pos)
		# for p in intersections:
		# 	self._path.addRect(p.x()-5, p.y()-5, 10, 10)

		self._hull = QPolygonF()

		hs = connection_selected_pen.width() * 2 # hull shift
		self._hull.append(start_pos - QPoint(0, hs))

		for p in points:
			self._hull.append(p - QPoint(0, hs))

		for p in points[::-1]:
			self._hull.append(p + QPoint(0, hs))

		self._hull.append(start_pos + QPoint(0, hs))

		return self._path

	def getPath(self):
		return self._path

	def _drawHalfCircle(self, segA, segB, p):
		x, y = p.x(), p.y()
		angle = (270-angleBetween(*segA))

		start_angle = angle % 270
		end_angle = start_angle + 180

		if angle > 270:
			start_angle += 90
			end_angle += 90

		angles = np.radians(np.arange(start_angle, end_angle))
		pts = list(zip(np.cos(angles) * wire_skip_size, np.sin(angles) * wire_skip_size))

		self._path.lineTo(QPoint(*pts[0]) + p)
		for circle_point in pts:
			self._path.lineTo(QPoint(*circle_point) + p)