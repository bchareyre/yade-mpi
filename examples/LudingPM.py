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
Chi2 = 0.69
Chi3 = 1.1
Chi4 = 1.39
#=======================================

particleMass = 4.0/3.0*math.pi*r1*r1*r1*rho
Vi1 = math.sqrt(k1/particleMass)*DeltaPMax*Chi1
Vi2 = math.sqrt(k1/particleMass)*DeltaPMax*Chi2
Vi3 = math.sqrt(k1/particleMass)*DeltaPMax*Chi3
Vi4 = math.sqrt(k1/particleMass)*DeltaPMax*Chi4

PhiF1 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)
PhiF2 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)
PhiF3 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)
PhiF4 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)

#=======================================

mat1 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=k1, kp=kp, kc=kc, PhiF=PhiF1, G0 = 0.0))
mat2 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=k1, kp=kp, kc=kc, PhiF=PhiF2, G0 = 0.0))
mat3 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=k1, kp=kp, kc=kc, PhiF=PhiF3, G0 = 0.0))
mat4 = O.materials.append(LudingMat(frictionAngle=fr,density=rho,k1=k1, kp=kp, kc=kc, PhiF=PhiF4, G0 = 0.0))

id11 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True))
id12 = O.bodies.append(sphere(center=[0,0,(r1 + r2)],radius=r2,material=mat1,fixed=False))

id21 = O.bodies.append(sphere(center=[0,3.0*r1,0],radius=r1,material=mat2,fixed=True))
id22 = O.bodies.append(sphere(center=[0,3.0*r1,(r1 + r2)],radius=r2,material=mat2,fixed=False))

id31 = O.bodies.append(sphere(center=[0,6.0*r1,0],radius=r1,material=mat3,fixed=True))
id32 = O.bodies.append(sphere(center=[0,6.0*r1,(r1 + r2)],radius=r2,material=mat3,fixed=False))

id41 = O.bodies.append(sphere(center=[0,9.0*r1,0],radius=r1,material=mat4,fixed=True))
id42 = O.bodies.append(sphere(center=[0,9.0*r1,(r1 + r2)],radius=r2,material=mat4,fixed=False))


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
]

O.bodies[id12].state.vel=[0,0,-Vi1]
O.bodies[id22].state.vel=[0,0,-Vi2]
O.bodies[id32].state.vel=[0,0,-Vi3]
O.bodies[id42].state.vel=[0,0,-Vi4]

def addPlotData(): 
  f1 = [0,0,0]
  f2 = [0,0,0]
  f3 = [0,0,0]
  f4 = [0,0,0]
  
  try:
    f1=O.forces.f(id12)
    f2=O.forces.f(id22)
    f3=O.forces.f(id32)
    f4=O.forces.f(id42)
  except:
    f1 = [0,0,0]
    f2 = [0,0,0]
    f3 = [0,0,0]
    f4 = [0,0,0]
  
  s1 = (O.bodies[id12].state.pos[2]-O.bodies[id11].state.pos[2])-(r1 + r2)
  s2 = (O.bodies[id22].state.pos[2]-O.bodies[id21].state.pos[2])-(r1 + r2)
  s3 = (O.bodies[id32].state.pos[2]-O.bodies[id31].state.pos[2])-(r1 + r2)
  s4 = (O.bodies[id42].state.pos[2]-O.bodies[id41].state.pos[2])-(r1 + r2)
  
    
  plot.addData(fc1=f1[2], sc1=-s1, fc2=f2[2], sc2=-s2, fc3=f3[2], sc3=-s3, fc4=f4[2], sc4=-s4)

plot.plots={'sc1':('fc1'), 'sc2':('fc2'), 'sc3':('fc3'), 'sc4':('fc4')}; plot.plot()

from yade import qt
qt.View()

O.run(320000)
O.wait()
plot.saveGnuplot('sim-data_LudigPM')
