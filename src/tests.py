from interpreter import Interpreter

test_string = '''
shit_line
a   = input('title', kd)
b   =     input('title' ,   kd)
c=input('title'    , kd)

'''

inputs = Interpreter.extractInputs(test_string)