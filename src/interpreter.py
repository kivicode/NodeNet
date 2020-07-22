from io_pins import *
import re

class Interpreter:

	extract_params_regex = r'([^,]+\(.+?\))|([^,]+)'
	find_function_call_regex = r'\b%s+\((.*)\)$'

	@staticmethod
	def extractIO(code, regex, IOClass, target_parent=None):
		pins = []
		lines = code.split('\n')
		for line in lines:
			params = re.findall(regex, line) # extract argument line (need separetion)

			if len(params) > 0: # at least varname and one arg
				params = re.findall(Interpreter.extract_params_regex, params[0])
				params = [str(eval(p[1].strip())) for p in params]
				print(params)
				pins += [IOClass(*params[:2], _parent=target_parent)]
		return pins

	@staticmethod
	def extractInputs(code, target_parent=None):
		regex = Interpreter.find_function_call_regex % 'input'
		return Interpreter.extractIO(code, regex, Input, target_parent=target_parent)

	@staticmethod
	def extractOutputs(code, target_parent=None):
		regex = Interpreter.find_function_call_regex % 'output'
		return Interpreter.extractIO(code, regex, Output, target_parent=target_parent)