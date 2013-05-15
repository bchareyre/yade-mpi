""" Generate random periodic sphere packing using SpherePack::makeCloud """
from yade import pack
p=pack.SpherePack()
O.cellSize=Vector3(10,10,10)
print p.makeCloud(Vector3(0,0,0),Vector3(10,10,10),.5,.5,200,True)
for s in p:
	O.bodies.append(sphere(s[0],s[1]))
O.engines=[BoundDispatcher([Bo1_Sphere_Aabb()])]
from yade import qt
qt.View()
