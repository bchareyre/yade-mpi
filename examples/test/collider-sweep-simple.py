O.bodies.append(sphere([0,0,10],.5))
#O.bodies.append(sphere([0,0,0],.5,fixed=True))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],label='collider'),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.1,gravity=[0,0,-1e4])
]
collider.verletDist=.5
O.dt=8e-2*PWaveTimeStep()
O.saveTmp()
from yade import timing
O.timingEnabled=True
from yade import qt
r=qt.Renderer()
#r['Body_bounding_volume']=True
v=qt.View(); qt.Controller()
v.ortho=True; #v.viewDir=O.bodies[0].phys.pos; v.lookAt=O.bodies[0].phys.pos;  v.upVector=(0,0,1); 
O.run(2,True)
