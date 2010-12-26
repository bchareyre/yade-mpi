from yade import pack,plot

utils.readParamsFromTable(useL3Geom=True,nonviscDamp=0,frictAngle=0,useClumps=False,noTableOk=True)
from yade.params import table

if 1:
	sp=pack.SpherePack();
	# bunch of balls, with an infinite plane just underneath
	if not table.useClumps: sp.makeCloud((0,0,0),(1,1,1),.05,.5);
	# use clumps of 2 spheres instead, to have rotation without friction 
	else: sp.makeClumpCloud((0,0,0),(1,1,1),[pack.SpherePack([((0,0,0),.05),((0,0,.08),.02)])],periodic=False)
	sp.toSimulation()
else: O.bodies.append(utils.sphere((0,0,2),radius=.5)) # one single bouncing ball
O.bodies.append(utils.wall(position=0,axis=2,sense=1))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Wall_Aabb()]),
	InteractionLoop([Ig2_Sphere_Sphere_L3Geom(approxMask=0),Ig2_Wall_Sphere_L3Geom(approxMask=0)],[Ip2_FrictMat_FrictMat_FrictPhys(frictAngle=table.frictAngle)],[Law2_L3Geom_FrictPhys_ElPerfPl(noSlip=False,noBreak=False)]) if table.useL3Geom else InteractionLoop([Ig2_Sphere_Sphere_ScGeom(),Ig2_Wall_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys(frictAngle=table.frictAngle)],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=table.nonviscDamp,kinSplit=True),
	PyRunner(iterPeriod=1,command='addPlotData()'),
]
O.dt=.1*utils.PWaveTimeStep()

def addPlotData():
	Ek,maxId=utils.kineticEnergy(findMaxId=True)
	plot.addData(i=O.iter,total=O.energy.total(),maxId=maxId,**O.energy)

# turn on energy tracking
O.trackEnergy=True
O.saveTmp()
# the callable should return list of strings, plots will be updated automatically
plot.plots={'i':[O.energy.keys,None,'total']}

#from yade import timing
#O.timingEnabled=True
#timing.stats()
plot.plot(subPlots=True)
