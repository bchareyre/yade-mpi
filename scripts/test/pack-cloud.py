""" Generate random periodic sphere packing using SpherePack::makeCloud """
from yade import pack
p=pack.SpherePack()
O.cellSize=Vector3(10,10,10)
print p.makeCloud(Vector3().ZERO,O.cellSize[1],.5,.5,1200,True)
for s in p:
	O.bodies.append(utils.sphere(s[0],s[1]))
O.runEngine(BoundDispatcher([Bo1_Sphere_Aabb()]))
from yade import qt
qt.View()
