# -*- coding: utf-8

# Testing sphere-sphere interaction in periodic case.
# Pass, if the spheres moves along the X axis, interacting through the period.

from yade import utils

sphereRadius=0.1
tc=0.001# collision time 
en=1  # normal restitution coefficient
es=1  # tangential restitution coefficient
density=2700
frictionAngle=radians(35)# 
params=utils.getViscoelasticFromSpheresInteraction(tc,en,es)
sphereMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,**params))


# Spheres
sphId=O.bodies.append([
	utils.sphere( (0.4,0.5,0.5), 0.1, material=sphereMat),
	utils.sphere( (0.6,0.5,0.5), 0.1, material=sphereMat)
	])
O.bodies[sphId[-1]].state.vel=(0.5,0,0)
O.bodies[sphId[0]].state.vel=(-0.5,0,0)

## Engines 
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0),
]

O.periodic=True
O.cell.refSize=Vector3(1,1,1)

O.dt=.01*tc

O.saveTmp()

