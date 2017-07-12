from yade import pack,plot
sp=pack.SpherePack()
rMean=.05
sp.makeCloud((0,0,0),(1,1,1),rMean=rMean,periodic=True)
sp.toSimulation()
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=.05*rMean),
	InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	NewtonIntegrator(damping=.6),
	PeriTriaxController(goal=(-1e6,-1e6,-.1),stressMask=0b011,maxUnbalanced=.2,doneHook='goalReached()',label='triax',maxStrainRate=(.1,.1,.1),dynCell=True),
	PyRunner(iterPeriod=100,command='addPlotData()')
]
O.dt=.5*utils.PWaveTimeStep()
O.trackEnergy=True
def goalReached():
	print 'Goal reached, strain',triax.strain,' stress',triax.stress
	O.pause()
def addPlotData():
	plot.addData(sx=triax.stress[0],sy=triax.stress[1],sz=triax.stress[2],ex=triax.strain[0],ey=triax.strain[1],ez=triax.strain[2],
		i=O.iter,unbalanced=utils.unbalancedForce(),
		totalEnergy=O.energy.total(),**O.energy       # plot all energies
	)
plot.plots={'i':(('unbalanced','go'),None,'kinetic'),' i':('ex','ey','ez',None,'sx','sy','sz'),'i ':(O.energy.keys,None,('totalEnergy','bo'))}
plot.plot()
O.saveTmp()
O.run()

