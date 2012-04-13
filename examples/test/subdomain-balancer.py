from yade import pack,timing,utils
utils.readParamsFromTable(noTableOk=True,num=12000)
import yade.params.table
sp=pack.SpherePack()
sp.makeCloud((0,0,0),(1,1,1),.03*((12000./yade.params.table.num)**(1/3.)),.5)
sp.toSimulation()
O.bodies.append(utils.wall((0,0,0),axis=2))
O.bodies.append(utils.wall((0,0,0),axis=1))
#O.bodies.append(utils.wall((0,0,0),axis=0))
#O.bodies.append(utils.wall((0,2,0),axis=1))
#O.bodies.append(utils.wall((2,0,0),axis=0))
O.engines=([SubdomainBalancer(axesOrder='xyz',colorize=True)] if 'SubdomainBalancer' in dir() else [])+[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Wall_Aabb()],verletDist=.05*.05),
	InteractionLoop([Ig2_Sphere_Sphere_ScGeom(),Ig2_Wall_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	NewtonIntegrator(gravity=(0,0,-10)),
	#PyRunner(iterPeriod=5000,command='O.pause(); timing.stats();')
]
O.dt=utils.PWaveTimeStep()
O.timingEnabled=True
#O.step(); #O.run(10000,True)
timing.stats()
from yade import qt
qt.View()
#O.step()
#O.run(5000,True)
