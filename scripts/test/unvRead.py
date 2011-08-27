from yade import *
from yade import ymport

facets = ymport.unv('shell.unv')
O.bodies.append([f for f in facets])

try:
	import qt
	qt.View()
except:
	pass
