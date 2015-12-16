#!/usr/bin/env python
# encoding: utf-8
from yade import utils, plot
o = Omega()

fr = 0.5;rho=2000

o.dt = 0.00002

r1 = 2.0e-1
r2 = 2.0e-1
#=======================================
k1 = 105.0
kp = 5.0*k1
kc = k1

DeltaPMax = 0.031
Chi1 = 0.34
#=======================================

particleMass = 4.0/3.0*math.pi*r1*r1*r1*rho
Vi1 = math.sqrt(k1/particleMass)*DeltaPMax*Chi1

PhiF1 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)

#=======================================

mat1 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=k1, kp=kp, kc=kc, PhiF=PhiF1, G0 = 0.0))

id11 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True))
id12 = O.bodies.append(sphere(center=[0,0,(r1 + r2)],radius=r2,material=mat1,fixed=False))



o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_LudingMat_LudingMat_LudingPhys()],
    [Law2_ScGeom_LudingPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,0]),
  PyRunner(command='changeVelDir()',iterPeriod=6000,label='Cvel'),
  PyRunner(command='addPlotData()',iterPeriod=300),
]

O.bodies[id12].state.vel=[0,0,-Vi1]

def changeVelDir():
  O.bodies[id12].state.vel*=-1
  Cvel.iterPeriod = int(Cvel.iterPeriod*1.5)
  
def addPlotData(): 
  f1 = [0,0,0]
  
  try:
    f1=O.forces.f(id12)
  except:
    f1 = [0,0,0]
  
  s1 = (O.bodies[id12].state.pos[2]-O.bodies[id11].state.pos[2])-(r1 + r2)  
    
  plot.addData(fc1=f1[2], sc1=-s1)

plot.plots={'sc1':('fc1')}; plot.plot()

from yade import qt
qt.View()

O.run(150000)
O.wait()
plot.saveGnuplot('sim-data_LudigPM')
