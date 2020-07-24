from io_pins import *
import re

class Interpreter:

	extract_params_regex = r'([^,]+)'
	find_function_call_regex = r'(?:(?P<varname>.*)\ *=\ *)?%s+\((?P<params>.*)\)'
	extract_comments = r"(?P<inline>#.*\n?)"

	def __init__(self, parent):
		self.parent = parent

	def _extractIO(self, code, regex, IOClass, target_parent=None):
		pins = {}
		line_numbers = []
		indentions = []
		lines = code.split('\n')
		for i, line in enumerate(lines):
			line = self._removeComments(line)
			match = re.search(regex, line) # extract argument line and varname(need separetion)
			if match is None: continue
			varname = match.group('varname')
			if varname is None:
				indention = (line.rstrip().rfind('\t') + 1) * '\t' # remove possible right indention and find last
			else:
				indention = (varname.rfind('\t') + 1) * '\t' # varname contains indention and var name itself
			indentions.append(indention)
			params = match.group('params')
			if len(params) > 0: # at least varname and one arg
				params = re.findall(self.extract_params_regex, params)
				params = [str(p.strip()).replace('\'', '') for p in params]
				if varname is None: varname = params[0]
				varname = varname.strip()
				line_numbers.append(i)
				pins[varname] = IOClass(*params, _parent=self.parent)
		return pins, line_numbers, indentions

	def removeIOLines(self, code, line_numbers):
		lines = code.split('\n')
		for i in line_numbers[::-1]:
			lines.remove(lines[i])
		return '\n'.join(lines)

	def extractInputs(self, code):
		regex = Interpreter.find_function_call_regex % 'input'
		return self._extractIO(code, regex, Input)[:-1]

	def extractOutputs(self, code):
		regex = Interpreter.find_function_call_regex % 'output'
		return self._extractIO(code, regex, Output)

	@staticmethod
	def _removeComments(line):
		return re.sub(Interpreter.extract_comments, '', line)

	@staticmethod
	def isNumber(line):
		try:
			float(line)
			return True
		except ValueError:
			pass # can't cast to float -> not a number
		return False
