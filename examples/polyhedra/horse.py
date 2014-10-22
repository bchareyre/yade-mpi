# modified script yade/examples/gts-horse/horse.py
from yade import pack,ymport
import gts

surf=gts.read(open('horse.coarse.gts'))
pred=pack.inGtsSurface(surf)
aabb=pred.aabb()
dim0=aabb[1][0]-aabb[0][0]; radius=dim0/40.
O.bodies.append(pack.regularHexa(pred,radius=radius,gap=radius/4.))

tetras = ymport.ele('horse.node','horse.ele',shift=(0,0,-1*(aabb[1][2]-aabb[0][2])),wire=False,color=(0,1,1),fixed=True)
O.bodies.append(tetras)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Polyhedra_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Sphere_Polyhedra_ScGeom(edgeCoeff=.1,vertexCoeff=.05)],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.1,gravity=(0,0,-5000)),
	PyRunner(iterPeriod=10,command='addPlotData()')
]

O.dt=.4*PWaveTimeStep()
O.saveTmp()
O.timingEnabled=True
O.trackEnergy=True
from yade import plot
plot.plots={'i':('total',O.energy.keys,)}
def addPlotData(): plot.addData(i=O.iter,total=O.energy.total(),**O.energy)
plot.plot(subPlots=False)

from yade import qt
qt.View()
