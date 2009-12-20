# coding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>
"Test and demonstrate use of PeriTriaxController."
from yade import *
from yade import pack,log,qt
#log.setLevel('PeriTriaxController',log.DEBUG)
O.periodic=True
O.cell.refSize=Vector3(.1,.1,.1)
sp=pack.SpherePack()
radius=5e-3
num=sp.makeCloud(Vector3().ZERO,O.cell.refSize,radius,.2,500,periodic=True) # min,max,radius,rRelFuzz,spheresInCell,periodic
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])


O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(nBins=5,sweepLength=.05*radius),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()]
	),
	PeriTriaxController(goal=[-1e5,-1e5,0],stressMask=3,globUpdate=5,maxStrainRate=[1.,1.,1.],doneHook='triaxDone()',label='triax'),
	NewtonIntegrator(damping=.6),
]
O.dt=utils.PWaveTimeStep()
O.run();
qt.View()

phase=0
def triaxDone():
	global phase
	if phase==0:
		print 'Here we are: stress',triax['stress'],'strain',triax['strain'],'stiffness',triax['stiff']
		print 'Now εz will go from 0 to .2 while σx and σy will be kept the same.'
		triax['goal']=[-1e5,-1e5,.2]
		phase+=1
	elif phase==1:
		print 'Here we are: stress',triax['stress'],'strain',triax['strain'],'stiffness',triax['stiff']
		print 'Done, pausing now.'
		O.pause()
		
	

