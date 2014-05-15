#!/usr/bin/python
# -*- coding: utf-8 -*-
# This example shows, how one can simulate the conveyor of
# complex form, using mask-parameter in NewtonIntegrator.
# The elements of conveyor are getting the velocity, but
# are blocked from motion. 
# Conveyor element have a groupMask=5 which is after bitwise AND 
# operator with mask=2 in NewtonIntegrator gives 0 and prevents motion.
#
# So the interacting particles are getting motion from interaction 
# with conveyor

## PhysicalParameters 
Density=1000
frictionAngle=0.4
tc = 0.001
en = 0.3
es = 0.3


## Import wall's geometry
mat=O.materials.append(ViscElMat(density=Density,frictionAngle=frictionAngle,tc=tc,en=en,et=es))

sp=pack.SpherePack()
sp.makeCloud((-0.3,0.05,0.05),(0.3,0.7,0.5),rMean=0.03, rRelFuzz=0.001)
particles=O.bodies.append([sphere(c,r,mask=3) for c,r in sp])

from yade import ymport
fctIds= O.bodies.append(ymport.gmsh('conveyor.mesh',scale=0.001,color=(1,0,0)))
voxIds= O.bodies.append(utils.geom.facetBunker(center=[0,1.5,-0.7],dBunker=1.1, dOutput=0.2,hBunker=0.2,hOutput=0.2,hPipe=0.1, mask=5))

for i in fctIds:
	O.bodies[i].state.vel=Vector3(0,0.2,0)      # Set conveyor velocity

## Timestep 
O.dt=.2*tc

## Engines 
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0,gravity=[0,0,-9.81], mask=2),
	DomainLimiter(lo=(-0.6,0.0,-1.0),hi=(0.6,2.0,1.0),iterPeriod=200),
]

from yade import qt
qt.View()
#O.saveTmp()
#O.run()

