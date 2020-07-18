from node_pin_graph import QNEGraphicsPin
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from settings import *


LEFT_TOP = 1
LEFT_BOTTOM = 2
RIGHT_TOP = 3
RIGHT_BOTTOM = 4


class Pin():
    def __init__(self, node, index=0, position=LEFT_TOP):

        self.node = node
        self.index = index
        self.position = position

        self.graphNode = QNEGraphicsPin(self.node.graphNode)

        self.graphNode.setPos(*self.node.getNodePosition(index, position))
