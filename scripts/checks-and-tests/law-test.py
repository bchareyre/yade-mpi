#
# demonstrate capabilities of LawTester
#
# The contact is loaded as described by LawTester.path triplets:
#
# 1. Push spheres together, then shear, then release in the normal sense,
#    while keeping shear constant; slipping is taking place along the
#    Mohr-Coulomb plasticity surface
#
# 2. Load in the normal direction, then go around a square in the shear plane
#
# The sphere couple is oriented randomly, but the result should always be the same.
#
from yade import utils,plot
import random
random.seed()


# sphere's radii
r1,r2=.1,.2
# place sphere 1 at the origin
pt1=Vector3(0,0,0)
# random orientation of the interaction
normal=Vector3(random.random()-.5,random.random()-.5,random.random()-.5)
normal=Vector3.UnitX
O.bodies.append([
	utils.sphere(pt1,r1,wire=True,color=(.7,.7,.7)),
	utils.sphere(pt1+.999999*(r1+r2)*normal.normalized(),r2,wire=True,color=(0,0,0))
])

O.engines=[
	ForceResetter(),
	PyRunner(iterPeriod=1,command='import time; time.sleep(.05)'),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		#[Ig2_Sphere_Sphere_ScGeom()],	[Ip2_FrictMat_FrictMat_FrictPhys()], [Law2_ScGeom_FrictPhys_CundallStrack()] # ScGeom
		#[Ig2_Sphere_Sphere_L3Geom(approxMask=63)],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl(noBreak=True,noSlip=False)] # L3Geom
		[Ig2_Sphere_Sphere_L6Geom(approxMask=63)],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L6Geom_FrictPhys_Linear(charLen=1)] # L6Geom
	),
	LawTester(ids=[0,1],disPath=[(0,0,0)]*7+[(-1e-5,0,0),(-1e-5,.1,.1)],rotPath=[(0,.2,0),(0,0,0),(0,0,.2),(0,0,0),(.2,0,0),(-.2,0,0),(0,0,0)],pathSteps=[10],doneHook='tester.dead=True; O.pause();',label='tester',rotWeight=0),
	#LawTester(ids=[0,1],path=[
	#	(-1e-5,0,0),(-.1,0,0),(-.1,.1,0),(-1e-5,.1,0), # towards, shear, back to intial normal distance
	#	(-.02,.1,.1),(-.02,-.1,.1),(-.02,-.1,-.1),(-.02,.1,-.1),(-.02,.1,.1), # go in square in the shear plane without changing normal deformation
	#	(-1e-4,0,0) # back to the origin, but keep some overlap to not delete the interaction
	#	],pathSteps=[100],doneHook='tester.dead=True; O.pause()',label='tester',rotWeight=.2,idWeight=.2),
	NewtonIntegrator(),
	PyRunner(iterPeriod=1,command='addPlotData()'),
]

def addPlotData():
	i=O.interactions[0,1]
	plot.addData(
		un=tester.uTest[0],us1=tester.uTest[1],us2=tester.uTest[2],
		ung=tester.uGeom[0],us1g=tester.uGeom[1],us2g=tester.uGeom[2],
		phiX=tester.uTest[3],phiY=tester.uTest[4],phiZ=tester.uTest[5],
		phiXg=tester.uGeom[3],phiYg=tester.uGeom[4],phiZg=tester.uGeom[5],
		i=O.iter,Fs=i.phys.shearForce.norm(),Fn=i.phys.normalForce.norm(),Tx=O.forces.t(0)[0],Tyz=sqrt(O.forces.t(0)[1]**2+O.forces.t(0)[2]**2)
	)
plot.plots={'us1':('us2',),'Fn':('Fs',),'i':('un','us1','us2'),' i':('Fs','Fn','Tx','Tyz'),'  i':('ung','us1g','us2g'),'i  ':('phiX','phiXg','phiY','phiYg','phiZ','phiZg')}  #'ung','us1g','us2g'
plot.plot(subPlots=True)

try:
	from yade import qt
	qt.Controller(); v=qt.View()
	rr=qt.Renderer()
	rr.extraDrawers=[GlExtra_LawTester()]
	rr.intrGeom=True
except ImportError: pass
O.dt=1

O.saveTmp()
#O.run()
