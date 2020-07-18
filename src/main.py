from PyQt5.QtWidgets import *
import sys

from node_editor import NodeEditorWindow

if __name__ == '__main__':
	app = QApplication(sys.argv)

	window = NodeEditorWindow()

	sys.exit(app.exec_())