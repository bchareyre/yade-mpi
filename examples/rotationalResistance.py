#!/usr/bin/env python
# encoding: utf-8
from yade import utils, plot
o = Omega()
fr = 0.5;rho=2000
tc = 0.001; en = 0.7; et = 0.7; o.dt = 0.0002*tc

r = 0.002

param = getViscoelasticFromSpheresInteraction(tc,en,et)

mat1 = O.materials.append(ViscElMat(frictionAngle=fr,mR = 0.05, mRtype = 1, density=rho,**param))
mat2 = O.materials.append(ViscElMat(frictionAngle=fr,mR = 0.05, mRtype = 2, density=rho,**param))

oriBody = Quaternion(Vector3(1,0,0),(pi/28))

id1 = O.bodies.append(sphere(center=[0,0,2*r],radius=r,material=mat1))
id2 = O.bodies.append(geom.facetBox(center=(0,-16.0*r,-2*r),orientation=oriBody,extents=(r,17.0*r,0), material=mat1,color=(0,0,1)))

id3 = O.bodies.append(sphere(center=[10*r,0,2*r],radius=r,material=mat2))
id4 = O.bodies.append(geom.facetBox(center=(10*r,-16.0*r,-2*r),orientation=oriBody,extents=(r,17.0*r,0), material=mat2,color=(0,0,1)))

o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
  PyRunner(command='addPlotData()',iterPeriod=10000, dead = False, label='graph'),
]

def addPlotData(): 
  f1 = [0,0,0]
  s1 = O.bodies[id1].state.pos[1]
  s2 = O.bodies[id3].state.pos[1]
  plot.addData(sc=O.time, fc1=s1, fc2=s2)
  
  

plot.plots={'sc':('fc1','fc2')}; plot.plot()

from yade import qt
qt.View()
