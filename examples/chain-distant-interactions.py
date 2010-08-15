#!/usr/bin/python
# -*- coding: utf-8 -*-


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionDispatchers(
		## Create geometry information about each potential collision.
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		## Create physical information about the interaction.
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		## Constitutive law
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	TorqueEngine(subscribedBodies=[1],moment=(0,1000,0)),
	GravityEngine(gravity=(0,0,1e-2)),
	NewtonIntegrator(damping=0.2)
]

from yade import utils
from math import *
for n in range(5):
	O.bodies.append(utils.sphere([0,n,0],.5,dynamic=(n>0),color=[1-(n/20.),n/20.,0]))
	# looks for metaengine found in Omega() and uses those
	if n>0: utils.createInteraction(n-1,n)
for i in O.interactions: i.phys['ks']=1e7


O.dt=utils.PWaveTimeStep()
O.saveTmp('init')

try:
	from yade import qt
	qt.View()
	renderer=qt.Renderer()
	renderer.wire=True
	renderer.intrGeom=True
except ImportError: pass
