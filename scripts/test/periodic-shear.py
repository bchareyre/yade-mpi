O.cellSize=Vector3(1,1,1)
O.bodies.append(utils.sphere([0,0,0],.05))
g=0.
import yade.qt,time
v=yade.qt.View()
v.axes=True
v.grid=(True,True,True)
while True:
	O.cellShear=Vector3(.2*sin(g),.2*cos(pi*g),.2*sin(2*g)+.2*cos(3*g))
	time.sleep(0.001)
	g+=1e-3
