# encoding: utf-8
# © 2009 Václav Šmilauer <eudoxos@arcig.cz>
"""Script showing how to use GTS to import arbitrary triangulated surface,
which can further be either filled with packing (if used as predicate) or converted
to facets representing the surface."""

from yade import pack
import gts

try:
	surf=gts.read(open('horse.gts'))
	surf.coarsen(1000) # MUCH faster subsequently if the surface is coarse
except IOError:
	print """horse.gts not found, you need to download input data:

	wget http://gts.sourceforge.net/samples/horse.gts.gz
	gunzip horse.gts.gz
	"""
	quit()

if surf.is_closed():
	pred=pack.inGtsSurface(surf)
	aabb=pred.aabb()
	dim0=aabb[1][0]-aabb[0][0]; radius=dim0/30. # get some characteristic dimension, use it for radius
	O.bodies.append(pack.regularHexa(pred,radius=radius,gap=radius/4.,density=2000))
	surf.translate(0,0,-(aabb[1][2]-aabb[0][2])) # move surface down so that facets underneath
O.bodies.append(pack.gtsSurface2Facets(surf,wire=True))

O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom(),ef2_Facet_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[ef2_Dem3Dof_Elastic_ElasticLaw()],
	),
	GravityEngine(gravity=[0,0,-1e4]),
	NewtonsDampedLaw(damping=.1)
]

O.dt=1.5*utils.PWaveTimeStep()
O.saveTmp()
