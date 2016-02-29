#!/usr/bin/env python
# encoding: utf-8

# 3 pairs of spheres with different diameters,
# different capillary bridge models and different
# bridge volume are pulled out and the capillary forces
# are messed after 250 iterations and compared to a
# verified values (TODO: values check one more time). 

# Links to papers:
# Comparison of different capillary bridge models for application in the discrete element method
# (A. Gladkyy, R. Schwarze)
# http://arxiv.org/pdf/1403.7926.pdf

# Capillary forces between surfaces with nanoscale roughness (Yakov I. Rabinovich ...)
# http://diyhpl.us/~bryan/papers2/Capillary%20forces%20between%20surfaces%20with%20nanoscale%20roughness.pdf

from yade import utils, plot
o = Omega()
fr = 0.5;rho=2000
tc = 0.001; en = 0.7; et = 0.7; o.dt = 0.001*tc

tolerance = 0.0001

r0 = 19.0e-6
r1 = 35.0e-6
r2 = 32.5e-6
r3 = 27.5e-6

Gamma1 = 27.0e-3
Gamma2 = 24.0e-3
Gamma3 = 28.0e-3

Theta = 10.0

VB1 = 2.0e-19
VB2 = 12.0e-19
VB3 = 36.0e-19

CapillarType1 = "Willett_numeric"
CapillarType2 = "Willett_analytic"
CapillarType3 = "Rabinovich"

mat1 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB1,gamma=Gamma1,theta=Theta,Capillar=True,CapillarType=CapillarType1,tc=tc,en=en,et=et))
mat2 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB2,gamma=Gamma2,theta=Theta,Capillar=True,CapillarType=CapillarType2,tc=tc,en=en,et=et))
mat3 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB3,gamma=Gamma3,theta=Theta,Capillar=True,CapillarType=CapillarType3,tc=tc,en=en,et=et))


id1 = O.bodies.append(sphere(center=[0,0,0],radius=r0,material=mat1,fixed=True))
id2 = O.bodies.append(sphere(center=[0,0,(r0+r1)*0.99999],radius=r1,material=mat1,fixed=True))

id3 = O.bodies.append(sphere(center=[3.0*r1,0,0],radius=r0,material=mat2,fixed=True))
id4 = O.bodies.append(sphere(center=[3.0*r1,0,(r0+r2)*0.99999],radius=r2,material=mat2,fixed=True))

id5 = O.bodies.append(sphere(center=[6.0*r1,0,0],radius=r0,material=mat3,fixed=True))
id6 = O.bodies.append(sphere(center=[6.0*r1,0,(r0+r3)*0.99999],radius=r3,material=mat3,fixed=True))


o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=(r0+r1)*5.0),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_ViscElCapMat_ViscElCapMat_ViscElCapPhys()],
    [Law2_ScGeom_ViscElCapPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,0]),
]

O.bodies[id2].state.vel=[0,0,0.001]
O.bodies[id4].state.vel=[0,0,0.001]
O.bodies[id6].state.vel=[0,0,0.001]


O.run(250,True)

f1 = -O.forces.f(id2)[2]
f2 = -O.forces.f(id4)[2]
f3 = -O.forces.f(id6)[2]
O.wait()

if ((abs(2.07328148666e-07-f1)/f1)>tolerance):
  resultStatus += 1
  
if ((abs(6.78192074e-07-f2)/f2)>tolerance):
  resultStatus += 1
  
if ((abs(1.58617796928e-06-f3)/f3)>tolerance):
  resultStatus += 1

