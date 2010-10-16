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
import random, yade.log
#yade.log.setLevel('LawTester',yade.log.TRACE)

# sphere's radii
r1,r2=.1,.2
# place sphere 1 at the origin
pt1=Vector3(0,0,0)
# random orientation of the interaction
#normal=Vector3(random.random()-.5,random.random()-.5,random.random()-.5)
normal=Vector3(1,0,0)
O.bodies.append([
	utils.sphere(pt1,r1,wire=True,color=(.7,.7,.7)),
	utils.sphere(pt1+.999999*(r1+r2)*normal.normalized(),r2,wire=True,color=(0,0,0))
])

O.engines=[
	ForceResetter(),
	#PyRunner(iterPeriod=1,command='import time; time.sleep(.005)'),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		#[Ig2_Sphere_Sphere_ScGeom()],	[Ip2_FrictMat_FrictMat_FrictPhys()], [Law2_ScGeom_FrictPhys_CundallStrack()]
		[Ig2_Sphere_Sphere_L3Geom_Inc(approxMask=63)],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl(noBreak=True,noSlip=False)] # use this line for L3Geom
	),
	#LawTester(ids=[0,1],rotPath=[(.2,0,0),(-.2,0,0),(0,0,0),(0,.2,0),(0,0,0),(0,0,.2),(0,0,0),
	LawTester(ids=[0,1],path=[
		(-1e-5,0,0),(-.1,0,0),(-.1,.1,0),(-1e-5,.1,0), # towards, shear, back to intial normal distance
		(-.02,.1,.1),(-.02,-.1,.1),(-.02,-.1,-.1),(-.02,.1,-.1),(-.02,.1,.1), # go in square in the shear plane without changing normal deformation
		(-1e-4,0,0) # back to the origin, but keep some overlap to not delete the interaction
		],pathSteps=[100],doneHook='tester.dead=True; O.pause()',label='tester',rotWeight=.2,idWeight=.2),
	PyRunner(iterPeriod=1,command='addPlotData()'),
	NewtonIntegrator()
]

def addPlotData():
	i=O.interactions[0,1]
	plot.addData(
		un=tester.ptOurs[0],us1=tester.ptOurs[1],us2=tester.ptOurs[2],
		ung=tester.ptGeom[0],us1g=tester.ptGeom[1],us2g=tester.ptGeom[2],
		phiX=tester.rotOurs[0],phiY=tester.rotOurs[1],phiZ=tester.rotOurs[2],
		phiXg=tester.rotGeom[0],phiYg=tester.rotGeom[1],phiZg=tester.rotGeom[2],
		i=O.iter,Fs=i.phys.shearForce.norm(),Fn=i.phys.normalForce.norm()
	)
plot.plots={'us1':('us2',),'Fn':('Fs',),'i':('un','us1','us2'),' i':('Fs','Fn'),'  i':('ung','us1g','us2g'),'i  ':('phiX','phiXg','phiY','phiYg','phiZ','phiZg')}  #'ung','us1g','us2g'
plot.plot(subPlots=True)

try:
	from yade import qt
	qt.Controller(); v=qt.View()
	rr=qt.Renderer()
	rr.extraDrawers=[GlExtra_LawTester()]
except ImportError: pass
O.dt=1

O.saveTmp()
#O.run()
