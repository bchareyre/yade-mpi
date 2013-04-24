# peri3dController_triaxialCompression.py
# script, that explains funcionality and input parameters of Peri3dController on the example of
# triaxial compression.
#   Firstly, a hydrostatic preassure is applied, then a strain along z axis is increasing
# while x- and y- stress is constant
#   The simulation is run on rotated cell to enable localization and strain softening
# (you can also try simulation with command sp.toSimulation() with no rotation,
# in this case there is almost no difference, but in script peri3dController_shear,
# the cell rotation has significant effect)

from yade import pack,plot,qt

# define material
O.materials.append(FrictMat())

# create periodic assembly of particles
initSize=1.2
sp=pack.randomPeriPack(radius=.05,initSize=Vector3(initSize,initSize,initSize),memoizeDb='/tmp/packDb.sqlite')
angle=0
rot=Matrix3(cos(angle),0,-sin(angle), 0,1,0, sin(angle),0,cos(angle))
sp.toSimulation(rot=rot)

# plotting 
plot.live=False
plot.plots={'iter':('sx','sy','sz','syz','szx','sxy',),'iter_':('ex','ey','ez','eyz','ezx','exy',),'ez':('sz',)}
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
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	NewtonIntegrator(),
	Peri3dController(	goal=(-1e7,-1e7,-1e7, 0,0,0), # Vector6 of prescribed final values
							stressMask=0b000111,
							nSteps=500,
							doneHook='print "Hydrostatic load reached."; O.pause()',
							youngEstimation=.5e9, # needed, when only nonzero prescribed values are stress
							maxStrain=.5,
							label='p3d'
							),
	PyRunner(command='plotAddData()',iterPeriod=1),
]
O.run(); O.wait()

# second part, z-axis straining and constant transversal stress
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	NewtonIntegrator(),
	Peri3dController(	goal=(-1e7,-1e7,-4e-1, 0,0,0), # Vector6 of prescribed final values
							stressMask=0b000011,
							nSteps=1000,
							xxPath=[(0,1),(1,1)], # the first (time) zero defines the initial value of stress considered nonzero
							yyPath=[(0,1),(1,1)],
							doneHook='print "Simulation with Peri3dController finished."; O.pause()',
							maxStrain=.5,
							label='p3d',
							strain=p3d.strain, # continue from value reached in previous part
							stressIdeal=Vector6(-1e7,-1e7,0, 0,0,0), # continue from value reached in previous part
							),
	PyRunner(command='plotAddData()',iterPeriod=1),
]
O.run();O.wait()
plot.plot(subPlots=False)
