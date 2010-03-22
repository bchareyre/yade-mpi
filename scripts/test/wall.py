"""
Show basic wall functionality (infinite axis-aligned planes).
"""
from yade import utils
kw=dict(young=30e9,density=1000)
O.bodies.append([
	utils.wall(1,axis=2,sense=-1,**kw),
	utils.wall(-5,axis=0,sense=1,**kw),
	utils.wall(1,axis=1,**kw),
	utils.wall((1,0,0),0,**kw),
	utils.sphere([0,0,0],.5),
	utils.sphere([-5,-4,-3],.5,**kw)
])
Gl1_Wall(div=10)

from yade import qt
renderer=qt.Renderer()
renderer['Body_interacting_geom']=True
qt.Controller()
qt.View()


O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),Bo1_Wall_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom(),Ig2_Wall_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()],
	),
	GravityEngine(gravity=[1e2,1e2,1e2]),
	NewtonIntegrator(damping=0.01),
	]

O.dt=utils.PWaveTimeStep()
O.save('/tmp/a.xml')
O.saveTmp()

from yade import log
log.setLevel('Ig2_Wall_Sphere_Dem3DofGeom',log.TRACE)

O.run()
#O.bodies.append([
#	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],dynamic=False,color=[1,0,0],young=1e3),
#	utils.facet([[1,-1,0],[0,1,0,],[1,.5,.5]],dynamic=False,young=1e3)
#])
#import random

