class IOPin:

	def __init__(self, _parent, _title, _type, _dtype):
		self.parent = _parent
		self.title = _title
		self.type = _type
		self.dtype = _dtype

	def build(self):
		is_input = self.type == 'input'
		self.parent.addPin(self.title, is_input, self.dtype == 'layer', False)
		if is_input:
			self.parent.addPin(self.title, False, False, True)

	def setType(self, _type):
		sel.dtype = _type

	def setParent(self, _parent):
		sel.parent = _parent

	def __repr__(self):
		return f'<{self.__class__} title: \'{self.title}\'  dtype: \'{self.dtype}\'  parent:{self.parent}'

	def __str__(self):
		return self.__repr__()

class Input(IOPin):

	def __init__(self, _title, _dtype='const', _parent=None):
		super().__init__(_parent, _title, 'input', _dtype)


class Output(IOPin):

	def __init__(self, _title, _dtype='const', _parent=None):
		super().__init__(_parent, _title, 'output', _dtype)
