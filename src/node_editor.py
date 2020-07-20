from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from node import Node
from node_connection import Connection
from node_scene import Scene
from node_editor_scene import QNEGraphicsScene
from node_editor_viewport import QNEGraphicsView
from settings import *

class NodeEditorWindow(QWidget):

	def __init__(self, parent=None):
		super().__init__(parent)
		self.initUI()

	def initUI(self):
		self.setWindowTitle('Node Editor')
		self.setGeometry(200, 200, 800, 600)
		self.setStyleSheet("border:0px")

		self.layout = QVBoxLayout()
		self.layout.setContentsMargins(0, 0, 0, 0)
		self.setLayout(self.layout)

		# create scene
		self.scene = Scene()
		self.graphScene = self.scene.graphScene


		# create viewport
		self.view = QNEGraphicsView(self.graphScene, self)
		self.view.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.view.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.view.setMouseTracking(True)

		self.layout.addWidget(self.view)

		self.addDebugContent()

		self.show()

	def spawnNode(self, *args, **argv):
		Node(self.scene, *args, **argv)

	def spawnConnection(self, first, second, *args, **argv):
		# Node(self.scene, *args, **argv)
		Connection(self.scene, first, second)


	def addDebugContent(self):
		self.spawnNode(QPoint(-200, -10), title='Batch Norm')
		self.spawnNode(QPoint(-200, -200), title='Batch Norm')
		self.spawnNode(QPoint(200, 10), title='Batch Norm')
		# self.spawnConnection()

	def contextMenuEvent(self, event):
		contextMenu = QMenu(self)
		contextMenu.setStyleSheet("border-radius:4px")

		layerMenu = QMenu("Layer")
		spawn_inp = layerMenu.addAction("Input")
		spawn_dense = layerMenu.addAction("Dense")
		spawn_output = layerMenu.addAction("Output")

		normalizationMenu = QMenu("Normalization")
		spawn_batch_norm = normalizationMenu.addAction("Batch Norm")

		mathMenu = QMenu("Math")
		spawn_add = mathMenu.addAction("Add")
		spawn_sub = mathMenu.addAction("Substract")
		spawn_mul = mathMenu.addAction("Multiply")
		spawn_div = mathMenu.addAction("Divide")

		spawn_const = contextMenu.addAction("Constant")

		contextMenu.addSeparator()
		contextMenu.addMenu(layerMenu)
		contextMenu.addMenu(normalizationMenu)
		contextMenu.addMenu(mathMenu)

		action = contextMenu.exec_(self.mapToGlobal(event.pos()))
		pos = self.view.mapFromScene(event.pos()) - QPoint(1000, 800) # centerize
		pos += 10 * QPoint(grid_size, grid_size)

		if action == spawn_const:
			self.spawnNode(pos, title='Constant')		

		elif action == spawn_inp:
			self.spawnNode(pos, title='Input')
		elif action == spawn_dense:
			self.spawnNode(pos, title='Dense')
		elif action == spawn_output:
			self.spawnNode(pos, title='Output')

		elif action == spawn_batch_norm:
			self.spawnNode(pos, title='Batch Norm')

		elif action == spawn_add:
			self.spawnNode(pos, title='Add')
		elif action == spawn_sub:
			self.spawnNode(pos, title='Substract')
		elif action == spawn_mul:
			self.spawnNode(pos, title='Multiply')
		elif action == spawn_div:
			self.spawnNode(pos, title='Divide')