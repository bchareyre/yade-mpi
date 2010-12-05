from yade import pack,plot

useL3Geom=True

sp=pack.SpherePack();
# bunch of balls, with an infinite plane just underneath
if 1:
	if 1: sp.makeCloud((0,0,0),(1,1,1),.05,.5);
	# use clumps of 2 spheres instead, to have rotation without friction 
	else: sp.makeClumpCloud((0,0,0),(1,1,1),[pack.SpherePack([((0,0,0),.05),((0,0,.08),.02)])],periodic=False)
	sp.toSimulation()
	#if useL3Geom:
	#O.bodies.append(pack.regularHexa(pack.inAlignedBox((-1,-1,-1),(2,2,0)),.5,-.2,dynamic=False))
	O.bodies.append(utils.wall(position=0,axis=2))
else:
	#O.bodies.append(utils.sphere((0,0,1),.2))
	O.bodies.append(utils.sphere((0,0,0.1035818200000008),.2))
	O.bodies[0].state.vel,O.bodies[0].state.accel=(0,0,-4.188869999999981),(0,0,-9.8100000000000005)
	O.bodies.append(utils.sphere((0.30000000000000004,0.19282032302755092,-0.5),.5,dynamic=False))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Wall_Aabb()]),
	InteractionLoop([Ig2_Sphere_Sphere_L3Geom_Inc(approxMask=63),Ig2_Wall_Sphere_L3Geom_Inc(approxMask=63)],[Ip2_FrictMat_FrictMat_FrictPhys(frictAngle=None)],[Law2_L3Geom_FrictPhys_ElPerfPl(noSlip=False,noBreak=False)]) if useL3Geom else InteractionLoop([Ig2_Sphere_Sphere_ScGeom(),Ig2_Wall_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys(frictAngle=None)],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=.1,kinSplit=True),
	PyRunner(iterPeriod=1,command='addPlotData()'),
]
O.dt=.2*utils.PWaveTimeStep()

def addPlotData():
	Ek,maxId=utils.kineticEnergy(findMaxId=True)
	plot.addData(i=O.iter,total=O.energy.total(),maxId=maxId,**O.energy)

# turn on energy tracking
O.trackEnergy=True
O.saveTmp()
# run a bit to have all energy categories in O.energy.keys().

# The number of steps when all energy contributions are already non-zero
# is only empirical; you can always force replot by redefining plot.plots
# as below, closing plots and running plot.plot() again
#
# (unfortunately even if we were changing plot.plots periodically,
# plots would not pick up changes in plot.plots during live plotting)
#O.run(427,True)  
#print O.bodies[0].state.pos,O.bodies[0].state.vel,O.bodies[0].state.accel
O.run(5,True)
#print list(set(['total',]+O.energy.keys()+['gravWork','kinRot','kinTrans','plastDissip']))+[None,'maxId']
plot.plots={'i':list(set(['total',]+O.energy.keys()+['gravWork','kinRot','kinTrans','plastDissip','elastPotential']))} #+[None,'maxId']}
#plot.plots={'i':['total',]+O.energy.keys()+[None,'maxId']}

#O.run()

from yade import timing
O.timingEnabled=True
O.run(50000,True)
timing.stats()
plot.plot(subPlots=True)
