#!/usr/bin/env python
# encoding: utf-8
from yade import utils, plot
o = Omega()

fr = 0.5;rho=2000

o.dt = 0.000002

r1 = 10.0e-2
r2 = 10.0e-2

mat1 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=0.2, kp=0.9, kc=0.1,PhiF=0.01, G0 = 0.0))

id1 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True))
id2 = O.bodies.append(sphere(center=[0,0,(r1 + r2)],radius=r2,material=mat1,fixed=True))

iterN = 10000000

o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_LudingMat_LudingMat_LudingPhys()],
    [Law2_ScGeom_LudingPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,0]),
  PyRunner(command='addPlotData()',iterPeriod=1000),
  PyRunner(command='changeDirection()',iterPeriod=iterN,label="cDir")
]

velTmp = 0.001

O.bodies[id2].state.vel=[0,0,-velTmp]

def changeDirection():
  global iterN
  if (O.bodies[id2].state.vel[2]<0.0): 
    O.bodies[id2].state.vel*=-1.0
    cDir.iterPeriod = int(iterN/10000.0)
  elif (O.bodies[id2].state.pos[2]-O.bodies[id1].state.pos[2])-(r1 + r2) > 0.0:
    iterN = int(iterN*1.2)
    O.bodies[id2].state.vel[2]*=-1.0
    cDir.iterPeriod = iterN
    
def addPlotData(): 
  f = [0,0,0]
  sc = 0
  try:
    f=O.forces.f(id2)
  except:
    f = [0,0,0]
  
  s1 = (O.bodies[id2].state.pos[2]-O.bodies[id1].state.pos[2])-(r1 + r2)
  
  fc1 = f[2]
  sc1 = -s1/r1
    
  plot.addData(fc1=fc1, sc=sc1)

plot.plots={'sc':('fc1')}; plot.plot()

from yade import qt
qt.View()

plot.saveGnuplot('sim-data_LudigPM')
