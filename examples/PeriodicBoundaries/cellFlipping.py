# coding: utf-8
# 2017 Bruno Chareyre <bruno.chareyre~a~grenoble-inp.fr>
"Demonstrate cell flipping in periodic boundary conditions"
from yade import pack,qt,plot

O.periodic=True
O.cell.Hsize=Matrix3(0.1,0,0, 0,0.1,0, 0,0,0.1)
sp=pack.SpherePack()
radius=5e-3
num=sp.makeCloud((0,0,0),(.1,.1,.1),radius,.2,100,periodic=True)
O.bodies.append([sphere(s[0],s[1]) for s in sp])


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=.05*radius),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),	PeriTriaxController(dynCell=True,mass=0.2,maxUnbalanced=0.01,relStressTol=0.02,goal=[-1e4,-1e4,-1e4],stressMask=7,globUpdate=5,maxStrainRate=[1.,1.,1.],doneHook='triaxDone()',label='triax'),
	NewtonIntegrator(damping=.2),
]
O.dt=2.e-5
phase=0
def triaxDone():
	global phase
	if phase==0:
		print 'Here we are: stress',triax.stress,'strain',triax.strain,'stiffness',triax.stiff
		print 'Now shearing.'
		O.cell.velGrad=Matrix3(0,1,0, 0,0,0, 0,0,0)
		triax.stressMask=7
		triax.goal=[-1e4,-1e4,-1e4]
		phase+=1
		O.saveTmp()
		O.pause()

O.run(-1,True)

def addPlotData():
	plot.addData(t=O.time,gamma=O.cell.trsf[0,1], unb = unbalancedForce(),s=getStress()[0,1])
O.engines=O.engines+[PyRunner(command='addPlotData()',iterPeriod=20)]
plot.plots={'gamma':'unb'}
plot.plot()

O.engines=O.engines+[PyRunner(command='if O.cell.hSize[0,1]>O.cell.hSize[0,0]: flipCell()',iterPeriod=20)]

#now click play and watch the flips happening, the evolution of stress or unbalanced force should not show any discontinuity