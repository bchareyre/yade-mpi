from yade import ymport

facets = ymport.unv('shell.unv',shift=(100,200,300),scale=1000)
O.bodies.append([f for f in facets])

try:
	import qt
	qt.View()
except:
	pass
