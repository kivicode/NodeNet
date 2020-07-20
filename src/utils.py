from PyQt5.QtCore import *
import math

def minDist(p, pts):
	min_dst = 10e9
	min_point = p
	for point in pts:
		d = dist(point, p)
		if d < min_dst: 
			min_dst = d
			min_point = point
	return min_point

def dist(a, b):
	return math.sqrt((a.x() - b.x())**2 + (a.y() - b.y())**2)

def fromAngle(angle, length):
	angle = math.radians(angle)
	return QPoint(math.cos(angle) * length, math.sin(angle) * length)

def isOnLine(point, a, b):
	return dist(a, point) + dist(point, b) == dist(a, b)

def angleBetween(a, b):
	delta = b - a
	try:
		ratio = delta.y() / delta.x()
		return math.degrees(math.atan(ratio))
	except:
		pass
	return 90

def angleBetweenLines(lineA, lineB):
	# Get nicer vector form
	vA = QPoint((lineA[0].x()-lineA[1].x()), (lineA[0].y()-lineA[1].y()))
	vB = QPoint((lineB[0].x()-lineB[1].x()), (lineB[0].y()-lineB[1].y()))
	# Get dot prod
	def dot(a, b):
		return a.x()*b.x()+a.y()*b.y()
	dot_prod = dot(vA, vB)
	# Get magnitudes
	magA = dot(vA, vA)**0.5
	magB = dot(vB, vB)**0.5
	# Get cosine value
	cos_ = dot_prod/magA/magB
	# Get angle in radians and then convert to degrees
	angle = math.acos(dot_prod/magB/magA)
	# Basically doing angle <- angle mod 360
	ang_deg = math.degrees(angle)%360

	return ang_deg

def lineIntersection(line1, line2):
	xdiff = QPoint(line1[0].x() - line1[1].x(), line2[0].x() - line2[1].x())
	ydiff = QPoint(line1[0].y() - line1[1].y(), line2[0].y() - line2[1].y())

	def det(a, b):
		return a.x() * b.y() - a.y() * b.x()

	div = det(xdiff, ydiff)
	if div == 0:
		return None# raise Exception('lines do not intersect')

	d = QPoint(det(*line1), det(*line2))
	x = det(d, xdiff) / div
	y = det(d, ydiff) / div
	return QPoint(round(x), round(y))

def isPointOnSegment(p, q, r):
	if r.x() <= max(p.x(), q.x()) and r.x() >= min(p.x(), q.x()) and r.y() <= max(p.y(), q.y()) and r.y() >= min(p.y(), q.y()):
		return True
	return False

def orientation(p, q, r):
	# return 0/1/-1 for colinear/clockwise/counterclockwise

	val = ((q.y() - p.y()) * (r.x() - q.x())) - ((q.x() - p.x()) * (r.y() - q.y()))
	if val == 0 : return 0
	return 1 if val > 0 else -1

def intersects(seg1, seg2):
	# check if seg1 and seg2 intersect
	p1, q1 = seg1
	p2, q2 = seg2

	o1 = orientation(p1, q1, p2)
	# find all orientations
	o2 = orientation(p1, q1, q2)
	o3 = orientation(p2, q2, p1)
	o4 = orientation(p2, q2, q1)

	if o1 != o2 and o3 != o4:
		# check general case
		return True

	# check special cases
	if o1 == 0 and on_segment(p1, q1, p2) : return True
	if o2 == 0 and on_segment(p1, q1, q2) : return True
	if o3 == 0 and on_segment(p2, q2, p1) : return True
	if o4 == 0 and on_segment(p2, q2, q1) : return True

	return False

def roundTo(val, rate=1):
	k = 10**rate
	return int(val * k) / k