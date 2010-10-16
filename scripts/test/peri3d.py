from yade import pack,log
O.materials.append(CpmMat(neverDamage=False,young=1e7,frictionAngle=.5,G_over_E=.2,sigmaT=3e6,epsCrackOnset=1e-2,relDuctility=10))
sp=pack.randomPeriPack(.5,.5,Vector3(10,10,10),memoizeDb='/tmp/packDb.sqlite')
O.periodic=True
O.cell.refSize=sp.cellSize
O.bodies.append([utils.sphere(c,r) for c,r in sp])
O.dt=utils.PWaveTimeStep()
log.setLevel('Peri3dController',log.TRACE)
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=1.5,label='bo1s')]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom(distFactor=1.5,label='ig2ss')],
		#[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_Dem3DofGeom_FrictPhys_CundallStrack()]
		[Ip2_CpmMat_CpmMat_CpmPhys()],[Law2_Dem3DofGeom_CpmPhys_Cpm()]

	),
	NewtonIntegrator(homotheticCellResize=1,damping=.8),
	#Peri3dController(goal=Matrix3(-.1,0,0, 0,-.1,0, 0,0,-.1),stressMask=0,maxStrainRate=1e-1,label='p3d'),
	#Peri3dController(goal=Matrix3(0,.2,0, 0,0,0, 0,0,0),stressMask=0,maxStrainRate=1,label='p3d'),
	Peri3dController(goal=Matrix3(.2,0,0, 0,0,0, 0,0,0),stressMask=0b000110,maxStrainRate=.1,label='p3d'),
	PyRunner(iterPeriod=10,command='addData()')
]
O.step()
bo1s.aabbEnlargeFactor=ig2ss.distFactor=-1
zRot=-pi/4.
O.cell.trsf=Matrix3(cos(zRot),-sin(zRot),0,sin(zRot),cos(zRot),0,0,0,1)
O.saveTmp()
from yade import plot
def addData():
	plot.addData(sxx=p3d.stress[0,0],syy=p3d.stress[1,1],szz=p3d.stress[2,2],exx=p3d.strain[0,0],eyy=p3d.strain[1,1],ezz=p3d.strain[2,2],t=O.time)

plot.plots={'t':('sxx',None,'eyy','ezz')}#,'exx':('sxx'),}
from yade import qt
rrr=qt.Renderer()
rrr.intrPhys,rrr.shape=True,False
Gl1_CpmPhys.dmgLabel,Gl1_CpmPhys.colorStrainRatio=False,10
qt.View()
	
