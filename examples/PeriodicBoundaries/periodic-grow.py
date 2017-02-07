"""Script that shrinks the periodic cell progressively.
It prints strain and average stress (computed from total volume force)
once in a while."""

from yade import timing
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],allowBiggerThanPeriod=True),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.6)
]
import random
for i in xrange(250):
	O.bodies.append(sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.5+random.random()))
cubeSize=20
# absolute positioning of the cell is not important
O.periodic=True
O.cell.setBox(cubeSize,cubeSize,cubeSize)
O.dt=PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
O.run(200,True)
rate=-1e-3*cubeSize/(O.dt*200)*Matrix3.Identity
O.cell.velGrad=rate

print 'Please be patient...'

for i in range(0,25):
	O.run(2000,True)
	F,stiff=totalForceInVolume()
	dim=O.cell.refSize; A=Vector3(dim[1]*dim[2],dim[0]*dim[2],dim[0]*dim[1])
	avgStress=sum([F[i]/A[i] for i in 0,1,2])/3.
	print 'strain',(cubeSize-dim[0])/cubeSize,'avg. stress ',avgStress,'unbalanced ',unbalancedForce()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
