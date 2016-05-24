######################################################################
# Gravity deposition as a simple simulation illustrating creation of
# cohesive contacts and deleting the unwanted between different
# agglomerates
#
# Using CpmMat, but the same procedure can be used with any material
######################################################################
from yade import ymport

wall = O.bodies.append(wall((0,0,3),2))

O.materials.append(CpmMat(neverDamage=True,frictionAngle=0))

# load spheres from file, including information of their agglomerates ids
attrs = []
sp = ymport.textExt('/tmp/compressed.txt',format='x_y_z_r_attrs',attrs=attrs)
n = max(int(a[0]) for a in attrs)+1
colors = [randomColor() for _ in xrange(n)]
for s,a in zip(sp,attrs):
	aa = int(a[0])
	s.agglomerate = aa
	s.shape.color = colors[aa]
O.bodies.append(sp)

factor=1.5
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=factor,label='bo1aabbs'),Bo1_Wall_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=factor,label='ig2sss'),Ig2_Wall_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys(),Ip2_CpmMat_CpmMat_CpmPhys(cohesiveThresholdIter=1)],
		[Law2_ScGeom_FrictPhys_CundallStrack(),Law2_ScGeom_CpmPhys_Cpm()]
	),
	NewtonIntegrator(gravity=(0,0,-30)),
]
O.dt = PWaveTimeStep()

# create cohesive interactions, possible also between different agglomerates
O.step()
ig2sss.interactionDetectionFactor = bo1aabbs.aabbEnlargeFactor = 1

# delete the inter-agglomerate interactions
for i in O.interactions:
	b1,b2 = [O.bodies[ii] for ii in (i.id1,i.id2)]
	if b1.agglomerate != b2.agglomerate:
		O.interactions.erase(i.id1,i.id2)
O.step()

try:
	from yade import qt
	qt.View()
except:
	pass
