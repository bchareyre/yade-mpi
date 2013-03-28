#
# check visually whether swept bounding boxes work as expected
#

mat=O.materials.append(FrictMat(young=30e9,poisson=.2,density=4000,frictionAngle=.5))

O.bodies.append([
	facet([[1,0,0],[-1,2,0],[-1,-2,0]],material=mat),
	sphere([0,0,2],.5,color=(0,1,0),material=mat)
])
O.bodies[1].state.vel=Vector3(0,0,-1)
O.dt=PWaveTimeStep()

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],label='isc'),
	InteractionLoop(
		[Ig2_Facet_Sphere_ScGeom(),Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=0,gravity=[0,0,-100])
]

from yade import timing,qt
try:
	renderer=qt.Renderer()
	#renderer['Body_bounding_volume']=True
	qt.Controller(); qt.View()
except ImportError: pass

O.timingEnabled=True
O.saveTmp()
O.step()
