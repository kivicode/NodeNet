class IOPin:

	def __init__(self, _parent, _title, _type, _dtype, _value):
		self.parent = _parent
		self.title = _title
		self.type = _type
		self.dtype = _dtype
		self.value = _value

	def build(self, name):
		is_input = self.type == 'input'
		self.parent.addPin(self.title, is_input, self.dtype == 'layer', False, val=self.value, dtype=self.dtype, varname=name)
		if is_input:
			self.parent.addPin(self.title, False, False, True, val=self.value, varname=name)

	def setType(self, _type):
		sel.dtype = _type

	def setParent(self, _parent):
		sel.parent = _parent

	def __repr__(self):
		return f'<{self.__class__} title: \'{self.title}\'  dtype: \'{self.dtype}\'  parent:{self.parent}'

	def __str__(self):
		return self.__repr__()

class Input(IOPin):

	def __init__(self, _title, value, _dtype='const', _parent=None):
		super().__init__(_parent, _title, 'input', _dtype, value)


class Output(IOPin):

	def __init__(self, _title, value, _dtype='const', _parent=None):
		super().__init__(_parent, _title, 'output', _dtype, value)
