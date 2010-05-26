from yade import pack,log
sp=pack.randomPeriPack(.5,.5,Vector3(10,10,10),memoizeDb='/tmp/packDb.sqlite')
O.periodic=True
O.cell.refSize=sp.cellSize
O.bodies.append([utils.sphere(c,r) for c,r in sp])
O.dt=utils.PWaveTimeStep()
log.setLevel('Peri3dController',log.TRACE)
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()]
	),
	NewtonIntegrator(homotheticCellResize=1,damping=.4),
	#Peri3dController(goal=Matrix3(-.1,0,0, 0,-.1,0, 0,0,-.1),stressMask=0,maxStrainRate=1e-1,label='ctrl'),
	#Peri3dController(goal=Matrix3(0,.2,0, 0,0,0, 0,0,0),stressMask=0,maxStrainRate=1,label='ctrl'),
	Peri3dController(goal=Matrix3(-1e3,.1,0, 0,.1,0, 0,0,0),stressMask=0b0001,maxStrainRate=1,label='ctrl'),
	PeriodicPythonRunner(iterPeriod=10,command='addData()')
]
O.step()
from yade import plot
def addData():
	plot.addData(sxx=ctrl.stress[0,0],syy=ctrl.stress[1,1],szz=ctrl.stress[2,2],exx=ctrl.strain[0,0],eyy=ctrl.strain[1,1],ezz=ctrl.strain[2,2],t=O.time)

plot.plots={'exx':('sxx'),}
	
