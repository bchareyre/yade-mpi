# peri3dController_example1.py
# script, that explains funcionality and input parameters of Peri3dController

from yade import pack, plot

# create some material
O.materials.append(CpmMat(neverDamage=True,young=25e9,frictionAngle=.7,poisson=.2,sigmaT=3e6,epsCrackOnset=1e-4,relDuctility=30))

# create periodic assembly of particles
initSize=1.2
sp=pack.randomPeriPack(radius=.05,initSize=Vector3(initSize,initSize,initSize),memoizeDb='/tmp/packDb.sqlite')
sp.toSimulation()

# plotting 
#plot.live=False
plot.plots={'progress':('sx','sy','sz','syz','szx','sxy',),'progress_':('ex','ey','ez','eyz','ezx','exy',)}
def plotAddData():
	plot.addData(
		progress=p3d.progress,progress_=p3d.progress,
		sx=p3d.stress[0],sy=p3d.stress[1],sz=p3d.stress[2],
		syz=p3d.stress[3],szx=p3d.stress[4],sxy=p3d.stress[5],
		ex=p3d.strain[0],ey=p3d.strain[1],ez=p3d.strain[2],
		eyz=p3d.strain[3],ezx=p3d.strain[4],exy=p3d.strain[5],
	)

# in how many time steps should be the goal state reached
nSteps=4000

O.dt=PWaveTimeStep()/2
EnlargeFactor=1.5
EnlargeFactor=1.0
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=EnlargeFactor,label='bo1s')]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=EnlargeFactor,label='ig2ss')],
		[Ip2_CpmMat_CpmMat_CpmPhys()],[Law2_ScGeom_CpmPhys_Cpm()]),
	NewtonIntegrator(),
	Peri3dController(
							goal=(20e-4,-6e-4,0, -2e6,3e-4,2e6), # Vector6 of prescribed final values (xx,yy,zz, yz,zx,xy)
							stressMask=0b101100,    # prescribed ex,ey,sz,syz,ezx,sxy;   e..strain;  s..stress
							nSteps=nSteps, 			# how many time steps the simulation will last
							# after reaching nSteps do doneHook action
							doneHook='print "Simulation with Peri3dController finished."; O.pause()',

							# the prescribed path (step,value of stress/strain) can be defined in absolute values
							xxPath=[(465,5e-4),(934,-5e-4),(1134,10e-4)],
							# or in relative values
							yyPath=[(2,4),(7,-2),(11,0),(14,4)],
							# if the goal value is 0, the absolute stress/strain values are always considered (step values remain relative)
							zzPath=[(5,-1e7),(10,0)],
							# if ##Path is not explicitly defined, it is considered as linear function between (0,0) and (nSteps,goal)
							# as in yzPath and xyPath
							# the relative values are really relative (zxPath gives the same - except of the sign from goal value - result as yyPath)
							zxPath=[(4,2),(14,-1),(22,0),(28,2)],
							xyPath=[(1,1),(2,-1),(3,1),(4,-1),(5,1)],
							# variables used in the first step
							label='p3d'
							),
	PyRunner(command='plotAddData()',iterPeriod=1),
]

O.step()
bo1s.aabbEnlargeFactor=ig2ss.interactionDetectionFactor=1.

O.run(); #O.wait()
plot.plot(subPlots=False)
