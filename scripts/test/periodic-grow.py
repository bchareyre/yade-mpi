"""Script that shrinks the periodic cell progressively.
It prints strain and average stress (computed from total volume force)
once in a while."""
from yade import log,timing
log.setLevel("InsertionSortCollider",log.TRACE)
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	NewtonIntegrator(damping=.6,homotheticCellResize=1)
]
import random
for i in xrange(250):
	O.bodies.append(utils.sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.5+random.random()))
cubeSize=20
# absolute positioning of the cell is not important
O.periodic=True
O.cell.refSize=Vector3(cubeSize,cubeSize,cubeSize)
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
O.run(200,True)
rate=-1e-4*cubeSize/(O.dt*200)*Matrix3().IDENTITY
O.cell['velGrad']=rate
for i in range(0,25):
	O.run(2000,True)
	F,stiff=utils.totalForceInVolume()
	dim=O.cell.refSize; A=Vector3(dim[1]*dim[2],dim[0]*dim[2],dim[0]*dim[1])
	avgStress=sum([F[i]/A[i] for i in 0,1,2])/3.
	print 'strain',(cubeSize-dim[0])/cubeSize,'avg. stress ',avgStress,'unbalanced ',utils.unbalancedForce()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
