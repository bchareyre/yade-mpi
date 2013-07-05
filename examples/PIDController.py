#!/usr/bin/env python
# encoding: utf-8
from yade import utils, plot

  
o = Omega()
fr = 0.0;rho=2000
tc = 0.001; en = 0.3; et = 0.3; o.dt = 0.02*tc

param = getViscoelasticFromSpheresInteraction(tc,en,et)
mat1 = O.materials.append(ViscElMat(frictionAngle=fr, density=rho,**param))

spheresID = O.bodies.append(pack.regularHexa(pack.inCylinder((0,0,-2.0),(0,0,2.0),2.0),radius=0.2,gap=0.1,color=(0,1,0),material=mat1))

idWalls = O.bodies.append(geom.facetCylinder(center=(0.0,0.0,0.0),radius = 2.05, height = 4.0, wallMask=6, material=mat1, segmentsNumber = 20, color=(0,0,1)))
idTop = O.bodies.append(geom.facetCylinder(center=(0.0,0.0,0.0),radius = 2.05, height = 4.0, wallMask=1, material=mat1, segmentsNumber = 5, color=(1,0,0), wire=False))


o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],verletDist=1.0,label='collider'),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81],label='newtonInt'),
  TranslationEngine(translationAxis=[0,0,1],velocity=-2.0,ids=idTop,dead=False,label='translat'),
  
  CombinedKinematicEngine(ids=idTop,label='combEngine',dead=True) + 
    ServoPIDController(axis=[0,0,1],maxVelocity=2.0,iterPeriod=1000,ids=idTop,target=1.0e7,kP=1.0,kI=1.0,kD=1.0) + 
    RotationEngine(rotationAxis=(0,0,1), angularVelocity=10.0, rotateAroundZero=True, zeroPoint=(0,0,0)),
  PyRunner(command='addPlotData()',iterPeriod=1000, label='graph'),
  PyRunner(command='switchTranslationEngine()',iterPeriod=45000, nDo = 2, label='switchEng'),
]

from yade import qt
qt.View()
r=qt.Renderer()
r.bgColor=1,1,1  

def addPlotData():
  fMove = Vector3(0,0,0)
  
  for i in idTop:
    fMove += O.forces.f(i)
  
  plot.addData(z=O.iter, pMove=fMove[2], pFest=fMove[2])

def switchTranslationEngine():
  print "Switch from TranslationEngine engine to ServoPIDController"
  translat.dead = True
  combEngine.dead = False
  


plot.plots={'z':('pMove','pFest')}; plot.plot()
