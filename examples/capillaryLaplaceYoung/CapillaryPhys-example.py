#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
To run this script you need to have all 10 text files from https://yade-dem.org/wiki/CapillaryTriaxialTest
in the same folder as you run this script in console!

This script shows how to use Law2_ScGeom_CapillaryPhys_Capillarity. The user can switch between hertz and 
linear model by setting "model_type"
"""

model_type		= 1	#1=Hertz model with capillary forces, else linear model with capillary model

#some parameters:
shear_modulus 	= 1e5
poisson_ratio 	= 0.3
young_modulus	= 2*shear_modulus*(1+poisson_ratio)
friction		= 0.5
angle			= atan(friction)
local_damping 	= 0.01
viscous_normal	= 0.021
viscous_shear	= 0.8*viscous_normal
lowercorner		= Vector3(0,0,0)
uppercorner		= Vector3(0.002,0.002,0.004)

#creating a material (FrictMat):
id_SphereMat=O.materials.append(FrictMat(young=young_modulus,poisson=poisson_ratio,density=2500,frictionAngle=angle))
SphereMat=O.materials[id_SphereMat]

#generate particles:
sp=pack.SpherePack()
sp.makeCloud(lowercorner,uppercorner,.0002,rRelFuzz=.3)
O.bodies.append([sphere(c,r,material=SphereMat) for c,r in sp])

#generate boundary:
O.bodies.append(geom.facetBox(uppercorner/2,uppercorner/2,wire=True,fixed=True,material=SphereMat))

#define engines:
if model_type == 1:#hertz model with capillary forces
	O.engines=[
		ForceResetter(),
		InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
		InteractionLoop(
			[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
			[Ip2_FrictMat_FrictMat_MindlinCapillaryPhys(label='ContactModel')],#for hertz model only
			[Law2_ScGeom_MindlinPhys_Mindlin()]#for hertz model only
		),
		Law2_ScGeom_CapillaryPhys_Capillarity(capillaryPressure=10000),#for hertz model only
		NewtonIntegrator(damping=local_damping,gravity=(0,0,-9.81)),
	]
	ContactModel.betan=viscous_normal
	ContactModel.betas=viscous_shear
	ContactModel.useDamping=True
else:
	O.engines=[
		ForceResetter(),
		InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
		InteractionLoop(
			[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
			[Ip2_FrictMat_FrictMat_CapillaryPhys()],	#for linear model only
			[Law2_ScGeom_FrictPhys_CundallStrack()],	#for linear model only
		),
		Law2_ScGeom_CapillaryPhys_Capillarity(capillaryPressure=10000),#for linear model only
		NewtonIntegrator(damping=local_damping,gravity=(0,0,-9.81)),
	]

#set time step and run simulation:
O.dt=0.5*PWaveTimeStep()

from yade import qt
qt.View()
print('Press PLAY button')
#O.run(10000,True)

