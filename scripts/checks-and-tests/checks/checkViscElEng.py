#!/usr/bin/env python
# encoding: utf-8

# 2 spheres. One is fixed, another one failing from up.
# Calculate en (coefficient of restitution) and compare with precalculated value
# Coefficient of restitution is the ratio of speeds after and before an impact
# This check-simulation checks the correctness of ViscoElasticEngine

from yade import utils, plot
o = Omega()
fr = 0.5;rho=2000
tc = 0.001; en = 0.7; et = 0.7; o.dt = 0.0002*tc


r1 = 0.003
r2 = 0.002


mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho,tc=tc,en=en,et=et))


id1 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True))
id2 = O.bodies.append(sphere(center=[0,0,(r1+r2*2.0)],radius=r2,material=mat1,fixed=False))


o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=(r1+r2)*5.0),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
  PyRunner(command='addPlotData()',iterPeriod=100),
]

v0 = 0
en = 0
tolerance = 0.0001

def addPlotData(): 
  global v0, en
  v = O.bodies[id2].state.vel[2]

  if v0<=0 and v>0:
    en=-v/v0
    print ("Precalculated en value %.12f" % 0.736356797441)
    print ("Obtained en value %.12f" % en)
    O.pause()
  v0=v

O.run(1000000)
O.wait()

if ((abs(0.736356797441-en)/en)>tolerance):
  resultStatus += 1
