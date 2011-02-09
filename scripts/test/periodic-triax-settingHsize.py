# coding: utf-8
# 2011 ©Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
"Demonstrate the compression of a periodic cell with non-trivial initial geometry."
from yade import *
from yade import pack,log,qt
log.setLevel('PeriTriaxController',log.TRACE)
O.periodic=True

O.cell.hSize=Matrix3(0.1,0.1,0, 0,0.2,0, 0,0,0.1)
sp=pack.SpherePack()
num=sp.makeCloud(minCorner=Vector3().Zero, maxCorner=(0.1,0.2,0.1), rMean=-0.01,rRelFuzz=.2, num=500,periodic=True, porosity=0.52,distributeMass=False)
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	#We compress the packing isotropicaly first
	PeriTriaxController( dynCell=True,mass=0.2,maxUnbalanced=0.01, relStressTol=0.01,goal=(-1e4,-1e4,-1e4), stressMask=7,globUpdate=5, maxStrainRate=(1.,1.,1.), doneHook='triaxDone()', reversedForces=True ,label='triax'),
	NewtonIntegrator(damping=.2),
	#PyRunner(iterPeriod=500,command='print "strain: ",triax.strain," stress: ",triax.stress')
]
O.dt=utils.PWaveTimeStep()
qt.View()

phase=0
def triaxDone():
	global phase
	if phase==0:
		print 'Here we are: stress',triax.stress,'strain',triax.strain
		#Here we reset the transformation, the compressed packing corresponds to null strain
		O.cell.trsf=Matrix3.Identity
		print 'Now εzz will go from 0 to .4 while σxx and σyy will be kept the same.'
		triax.stressMask=3
		triax.goal=(-1e4,-1e4,-0.4)

		phase+=1
	elif phase==1:
		print 'Here we are: stress',triax.stress,'strain',triax.strain
		print 'Done, pausing now.'
		O.pause()
		
	

