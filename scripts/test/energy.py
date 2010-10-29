from yade import pack,plot


sp=pack.randomPeriPack(radius=.05,initSize=(1,1,1),rRelFuzz=.5,memoizeDb='/tmp/triaxPackCache.sqlite')
sp.toSimulation()

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	NewtonIntegrator(),
	PyRunner(iterPeriod=10,command='addPlotData()')
]
O.trackEnergy=True

def addPlotData(): plot.addData(i=O.iter,**O.energy)

plot.plots={'i':('elastPotential','plastDissip')}
plot.plot(subPlots=True)

O.dt=utils.PWaveTimeStep()
O.cell.velGrad=Matrix3(-.1,0,0, 0,.05,0, 0,0,.03);
O.run()

#for i in range(0,100):
#	O.run(10,True)
#	for k in O.energy.keys():
#		print k,O.energy[k]

