"""Script that shrinks the periodic cell progressively.
It prints strain and average stress (computed from total volume force)
once in a while."""
from yade import log,timing
log.setLevel("PeriodicInsertionSortCollider",log.TRACE)
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),MetaInteractingGeometry2AABB()]),
	PeriodicInsertionSortCollider(),  # this is important, obviously
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	NewtonsDampedLaw(damping=.6)
]
import random
for i in xrange(250):
	O.bodies.append(utils.sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.5+random.random(),density=1000))
cubeSize=20
# absolute positioning of the cell is not important
O.periodicCell=((-.5*cubeSize,-.5*cubeSize,0),(.5*cubeSize,.5*cubeSize,cubeSize))
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
step=.01
O.run(200,True)
for i in range(0,250):
	O.run(200,True)
	mn,mx=O.periodicCell
	step=(mx-mn); step=Vector3(.002*step[0],.002*step[1],.002*step[2])
	O.periodicCell=mn+step,mx-step
	if (i%10==0):
		F=utils.totalForceInVolume()
		dim=mx-mn; A=Vector3(dim[1]*dim[2],dim[0]*dim[2],dim[0]*dim[1])
		avgStress=sum([F[i]/A[i] for i in 0,1,2])/3.
		print 'strain',(cubeSize-dim[0])/cubeSize,'avg. stress ',avgStress,'unbalanced ',utils.unbalancedForce()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
