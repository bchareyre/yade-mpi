#!/usr/bin/python
# -*- coding: utf-8 -*-
# © 2009 Václav Šmilauer <eudoxos@arcig.cz>
"""Script showing how to use GTS to import arbitrary triangulated surface,
which can further be either filled with packing (if used as predicate) or converted
to facets representing the surface."""

from yade import pack
import gts, os.path

# coarsen the original horse if we have it
# do nothing if we have the coarsened horse already
if not os.path.exists('horse.coarse.gts'):
	if os.path.exists('horse.gts'):
		surf=gts.read(open('horse.gts')); surf.coarsen(1000); surf.write(open('horse.coarse.gts','w'))
	else:
		print """horse.gts not found, you need to download input data:

		wget http://gts.sourceforge.net/samples/horse.gts.gz
		gunzip horse.gts.gz
		"""
		quit()

surf=gts.read(open('horse.coarse.gts'))

O.materials.append(FrictMat(young=30e9,density=2000))

if surf.is_closed():
	pred=pack.inGtsSurface(surf)
	aabb=pred.aabb()
	dim0=aabb[1][0]-aabb[0][0]; radius=dim0/30. # get some characteristic dimension, use it for radius
	O.bodies.append(pack.regularHexa(pred,radius=radius,gap=radius/4.))
	surf.translate(0,0,-(aabb[1][2]-aabb[0][2])) # move surface down so that facets are underneath the falling spheres
O.bodies.append(pack.gtsSurface2Facets(surf,wire=True))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],label='collider'),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()],
	),
	GravityEngine(gravity=[0,0,-1e4]),
	NewtonIntegrator(damping=.1)
]
collider.sweepLength,collider.nBins,collider.binCoeff=.1*dim0/30.5,10,2
O.dt=1.5*utils.PWaveTimeStep()
O.saveTmp()
O.timingEnabled=True

from yade import qt
qt.View()

from yade import timing
O.run(10000,True)
timing.stats()
