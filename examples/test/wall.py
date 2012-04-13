"""
Show basic wall functionality (infinite axis-aligned planes).
"""
from yade import utils
O.materials.append(FrictMat(young=30e9,density=1000,poisson=.2,frictionAngle=.5))
O.bodies.append([
	utils.wall(1,axis=2,sense=-1),
	utils.wall(-5,axis=0,sense=1),
	utils.wall(1,axis=1),
	utils.wall((1,0,0),0),
	utils.sphere([0,0,0],.5),
	utils.sphere([-5,-4,-3],.5)
])
Gl1_Wall(div=10)

from yade import qt
qt.Controller()
qt.View()


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),Bo1_Wall_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom(),Ig2_Wall_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=0.01,gravity=[1e2,1e2,1e2]),
	]

O.dt=utils.PWaveTimeStep()
O.save('/tmp/a.xml')
O.saveTmp()

O.run()
#O.bodies.append([
#	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],dynamic=False,color=[1,0,0],young=1e3),
#	utils.facet([[1,-1,0],[0,1,0,],[1,.5,.5]],dynamic=False,young=1e3)
#])
#import random

