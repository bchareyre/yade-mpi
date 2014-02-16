#!/usr/bin/python
# -*- coding: utf-8 -*-

# The script was implemented to check the following task
# http://math.stackexchange.com/questions/658871/perfectly-centered-break-of-a-perfectly-aligned-pool-ball-rack/659318#659318

massBall   = 1.0
radiusBall = 1.0
rowsN = 5
velBall = 10.0

vBall = 4.0/3.0 * math.pi * math.pow(radiusBall, 3.0)
rhoBall = massBall/vBall


## PhysicalParameters 
Density=rhoBall
frictionAngle=0.0
kn = 10e+11
ks = 10e+11
cn = 0.0
cs = 0.0

## Import wall's geometry
param = {'kn':kn*2.0, 'ks':ks*2.0, 'cn':cn*2.0, 'cs':cs*2.0}
sphereMat=O.materials.append(ViscElMat(density=Density,frictionAngle=frictionAngle,**param))

## Spheres
sphereIds = []
for r in range(0,rowsN):
  yPos = r*pow(3.0, 1.0/2.0)*radiusBall
  for i in range(r+1):
    xPos = i*2*radiusBall - r*pow(radiusBall, 3.0/2.0)
    sphereIds.append(O.bodies.append(sphere([xPos,yPos,0.0],radiusBall,material=sphereMat)))

## schlagBall

schlagBall = O.bodies.append(sphere([0.0,-2.0*radiusBall,0.0],radiusBall,material=sphereMat))

O.bodies[schlagBall].state.vel=Vector3(0,velBall,0)
## Timestep 
O.dt=.01*PWaveTimeStep()

## Engines 
O.engines=[
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0),
]

from yade import qt
qt.View()
#O.run(int(0.3/O.dt), True)
