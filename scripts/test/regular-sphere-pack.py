from yade import pack

""" This script demonstrates how to use 2 components of creating packings:

1. packing generators pack.regularHexa, pack.regularOrtho etc. generate vertices and filter them
using predicates. (Note that this will be enhanced to irregular packings in the future)

2. predicates are functors returning True/False for points that are given by the packing generator.
Their names are mostly self-explanatory, see their docstrings for meaning of their arguments.

Predicates can be combined using set arithmetics to get their Intersection (p1 & p2), union (p1 | p2),
difference (p1 - p2) and symmetric difference (XOR, p1 ^ p2). This is demontrated on the head (which
has sphere taken off at the back) and on the body (which has cylidrical hole inside).

"""

rad,gap=.15,.02
rho=1e3
kw={'density':rho}

O.bodies.append(
	pack.regularHexa(
		(pack.inSphere((0,0,4),2)-pack.inSphere((0,-2,5),2)) # & pack.notInNotch((0,0,4),(1,0,1),(.1,0,.1)) # do not use this, it doesn't work yet
		,radius=rad,gap=gap,color=(0,1,0),density=10*rho) # head
	+[utils.sphere((.8,1.9,5),radius=.2,color=(.6,.6,.6),density=rho),utils.sphere((-.8,1.9,5),radius=.2,color=(.6,.6,.6),density=rho),utils.sphere((0,2.4,4),radius=.4,color=(1,0,0),density=rho)] # eyes and nose
	+[utils.facet([(12,0,-6),(0,12,-6,),(-12,-12,-6)],dynamic=False)] # ground
	+pack.regularHexa(pack.inCylinder((-1,2.2,3.3),(1,2.2,3.3),2*rad),radius=rad,gap=gap/3,color=(0.929,0.412,0.412),density=10*rho) #mouth
)

for part in [
	pack.regularHexa (
		pack.inAlignedBox((-2,-2,-2),(2,2,2))-pack.inCylinder((0,-2,0),(0,2,0),1),
		radius=1.5*rad,gap=2*gap,color=(1,0,1),**kw), # body,
	pack.regularOrtho(pack.inEllipsoid((-1,0,-4),(1,1,2)),radius=rad,gap=0,color=(0,1,1),**kw), # left leg
	pack.regularHexa (pack.inCylinder((+1,1,-2.5),(0,3,-5),1),radius=rad,gap=gap,color=(0,1,1),**kw), # right leg
	pack.regularHexa (pack.inHyperboloid((+2,0,1),(+6,0,0),1,.5),radius=rad,gap=gap,color=(0,0,1),**kw), # right hand
	pack.regularOrtho(pack.inCylinder((-2,0,2),(-5,0,4),1),radius=rad,gap=gap,color=(0,0,1),**kw) # left hand
	]: O.bodies.appendClumped(part)



try:
	from yade import qt
	qt.Controller()
except ImportError: pass

O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom(),ef2_Facet_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[ef2_Dem3Dof_Elastic_ElasticLaw()],
	),
	GravityEngine(gravity=(1e-2,1e-2,-1000)),
	NewtonsDampedLaw(damping=.1)
]
# we don't care about physical accuracy here, over-critical step is fine as long as the simulation doesn't explode
O.dt=2*utils.PWaveTimeStep()
O.saveTmp('init')
#for b in O.bodies: 
