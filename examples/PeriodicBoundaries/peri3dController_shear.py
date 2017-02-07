# peri3dController_shear.py
# script, that explains funcionality and input parameters of Peri3dController on the example of
# shear test with rotated periodic cell (that enables strain localization).
#   The simulation is run on rotated cell to enable localization and strain softening
# (you can also try simulation with different angles of rotation to pbtain different results.


from yade import pack,plot,qt

# define material
O.materials.append(CpmMat(young=25e9,poisson=.2,sigmaT=3e6,epsCrackOnset=1e-4,relDuctility=1e-6))

# create periodic assembly of particles
initSize=1.2
sp=pack.randomPeriPack(radius=.05,initSize=Vector3(initSize,initSize,initSize),memoizeDb='/tmp/packDb.sqlite')
angle=0.
#angle=pi/4
rot=Matrix3(cos(angle),-sin(angle),0, sin(angle),cos(angle),0, 0,0,1)
sp.toSimulation(rot=rot)

# plotting 
plot.live=False
plot.plots={'iter':('sx','sy','sz','syz','szx','sxy',),'iter_':('ex','ey','ez','eyz','ezx','exy',),'exy':('sxy',)}
def plotAddData():
	plot.addData(
		iter=O.iter,iter_=O.iter,
		sx=p3d.stress[0],sy=p3d.stress[1],sz=p3d.stress[2],
		syz=p3d.stress[3],szx=p3d.stress[4],sxy=p3d.stress[5],
		ex=p3d.strain[0],ey=p3d.strain[1],ez=p3d.strain[2],
		eyz=p3d.strain[3],ezx=p3d.strain[4],exy=p3d.strain[5],
	)

O.dt=PWaveTimeStep()/2

# define the first part of simulation, hydrostatic compression
enlargeFactor=1.5
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=enlargeFactor,label='bo1s')]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=enlargeFactor,label='ig2ss')],
		[Ip2_CpmMat_CpmMat_CpmPhys()],[Law2_ScGeom_CpmPhys_Cpm()]),
	NewtonIntegrator(),
	Peri3dController(	goal=(0,0,0, 0,0,5e-3), # Vector6 of prescribed final values
							stressMask=0b011111,
							nSteps=2000,
							doneHook='print "Simulation with Peri3dController finished."; O.pause()',
							maxStrain=.5,
							label='p3d'
							),
	PyRunner(command='plotAddData()',iterPeriod=1),
]
O.step()
bo1s.aabbEnlargeFactor=ig2ss.interactionDetectionFactor=1.0

renderer=qt.Renderer()
renderer.intrPhys,renderer.shape=True,False
Gl1_CpmPhys.dmgLabel=False
qt.View()
O.run()
#plot.plot()
