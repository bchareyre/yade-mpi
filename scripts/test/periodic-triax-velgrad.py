# coding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>
"Test and demonstrate use of PeriTriaxController."
from yade import *
from yade import pack,log,qt
#log.setLevel('PeriTriaxController',log.DEBUG)
#log.setLevel('Shop',log.TRACE)
O.periodic=True
O.cell.refSize=Vector3(.1,.1,.1)
O.cell.trsf=Matrix3().IDENTITY;

sp=pack.SpherePack()
radius=5e-3
num=sp.makeCloud(Vector3().ZERO,O.cell.refSize,radius,.6,-1,periodic=True) # min,max,radius,rRelFuzz,spheresInCell,periodic
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],nBins=5,sweepLength=.05*radius),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()]
	),
	#PeriTriaxController(goal=[-1e3,-1e3,-1e3],stressMask=7,globUpdate=5,maxStrainRate=[5.,5.,5.],label='triax'),
	NewtonIntegrator(damping=.6, homotheticCellResize=0),
	PyRunner(command='utils.flipCell()',iterPeriod=1000),
]
O.dt=0.5*utils.PWaveTimeStep()
O.run(1)
qt.View()
O.cell.velGrad=Matrix3(0,10,0,0,0,0,0,0,0)
#O.cell.velGrad=Matrix3(0,5,2,-5,0,0,-2,0,0)
O.saveTmp()
O.run();
rrr=qt.Renderer(); rrr['intrAllWire'],rrr['Body_interacting_geom']=True,False

phase=0
def triaxDone():
	global phase
	if phase==0:
		print 'Here we are: stress',triax['stress'],'strain',triax['strain'],'stiffness',triax['stiff']
		print 'Now εz will go from 0 to .2 while σx and σy will be kept the same.'
		triax['goal']=[-1e5,-1e5,.2]
		O.cell.velGrad=Matrix3(0,0,0,5,0,0, 0,0,0)
		phase+=1
	elif phase==1:
		print 'Here we are: stress',triax['stress'],'strain',triax['strain'],'stiffness',triax['stiff']
		print 'Done, pausing now.'
		O.pause()
	
	

