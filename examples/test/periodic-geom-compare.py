# coding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>
"Test and demonstrate use of PeriTriaxController."
from yade import *
from yade import pack
O.periodic=True
O.cell.setBox(.1,.1,.1)
O.cell.trsf=Matrix3().Identity;

sp=pack.SpherePack()
radius=5e-3
num=sp.makeCloud(Vector3().Zero,O.cell.refSize,radius,.6,-1,periodic=True) # min,max,radius,rRelFuzz,spheresInCell,periodic
sp.toSimulation()

# specify which family of geometry functors to use
utils.readParamsFromTable(noTableOk=True,geom='sc')
from yade.params.table import geom

if geom=='sc':
	loop=InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()])
elif geom=='d3d':
	loop=InteractionLoop([Ig2_Sphere_Sphere_Dem3DofGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_Dem3DofGeom_FrictPhys_CundallStrack()])
elif geom=='l3':
	loop=InteractionLoop([Ig2_Sphere_Sphere_L3Geom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl()])
elif geom=='l3a':
	loop=InteractionLoop([Ig2_Sphere_Sphere_L3Geom(approxMask=63)],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl()])
else: raise ValueError('geom must be one of sc, d3d, l3, l3a (not %s)'%geom)


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=.05*radius),
	loop,
	NewtonIntegrator(damping=.6),
]
O.dt=0.5*utils.PWaveTimeStep()
try:
	from yade import qt
	qt.View()
except: pass

O.cell.velGrad=Matrix3(-.1,.03,0, 0,-.1,0, 0,0,-.1)
O.saveTmp()
#O.run(10000,True);
#rrr=qt.Renderer(); rrr.intrAllWire,rrr.intrGeom=True,False

if utils.runningInBatch():
	O.timingEnabled=True
	O.run(300000,True)
	O.timingEnabled
	from yade import timing
	timing.stats()
	

