# -*- coding: utf-8 -*-

# Testing facet-sphere interaction.
# A facet is rotated around Z axis.  Test pass, if a sphere at (0,0) position is not moving (because in this case no transfer moment from the facet to the sphere), but a sphere at facet's edge moves with the facet (for this sphere blocked the rotation DOFs in order to remove rolling).

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
tc = 0.001
en = 0.3
es = 0.3

## Import wall's geometry
params=utils.getViscoelasticFromSpheresInteraction(tc,en,es)
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,**params)) 
sphereMat=O.materials.append(ViscElMat(density=Density,frictionAngle=frictionAngle,**params))

facetId=O.bodies.append(utils.facet( [ (-1,0,0), (1,1,0), (1,-1,0)], material=facetMat,color=(1,0,0)))

sphIds=O.bodies.append([
	utils.sphere( (0,0,0.1),0.1, material=sphereMat,color=(0,1,0)), 
	utils.sphere( (0.9,0,0.1),0.1, material=sphereMat,color=(0,1,0))
	])

O.bodies[sphIds[1]].state.blockedDOFs='XYZ'

## Timestep 
O.dt=.1*tc

## Engines 
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonIntegrator(damping=0),
	RotationEngine(ids=[facetId],rotationAxis=[0,0,1],rotateAroundZero=True,angularVelocity=0.1)
]

from yade import qt
qt.View()
O.saveTmp()
#O.run()

