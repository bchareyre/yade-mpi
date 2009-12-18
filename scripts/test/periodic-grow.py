"""Script that shrinks the periodic cell progressively.
It prints strain and average stress (computed from total volume force)
once in a while."""
from yade import log,timing
log.setLevel("InsertionSortCollider",log.TRACE)
O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	NewtonIntegrator(damping=.6)
]
import random
for i in xrange(250):
	O.bodies.append(utils.sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.5+random.random()))
cubeSize=20
# absolute positioning of the cell is not important
O.cellSize=Vector3(cubeSize,cubeSize,cubeSize)
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
step=.01
O.run(200,True)
for i in range(0,250):
	O.run(200,True)
	O.cellSize=O.cellSize*.998
	if (i%10==0):
		F,stiff=utils.totalForceInVolume()
		dim=O.cellSize; A=Vector3(dim[1]*dim[2],dim[0]*dim[2],dim[0]*dim[1])
		avgStress=sum([F[i]/A[i] for i in 0,1,2])/3.
		print 'strain',(cubeSize-dim[0])/cubeSize,'avg. stress ',avgStress,'unbalanced ',utils.unbalancedForce()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
