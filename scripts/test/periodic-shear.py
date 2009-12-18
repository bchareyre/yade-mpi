O.cellSize=Vector3(.5,.5,.5)
O.bodies.append(utils.sphere([.25,.25,.25],.075))
O.engines=[BoundDispatcher([Bo1_Sphere_Aabb()])]
O.step()
g=0.
import yade.qt,time
v=yade.qt.View()
v.axes=True
v.grid=(True,True,True)
while False:
	O.cellShear=Vector3(.2*sin(g),.2*cos(pi*g),.2*sin(2*g)+.2*cos(3*g))
	time.sleep(0.001)
	g+=1e-3
O.cellShear=(.1,0,0)
yade.qt.Renderer()['Body_bounding_volume']=True
