######################################################################
# Compress the loose packing into dense packing. Each agglomerate is
# considered as clump in this stage
######################################################################
from yade import export,ymport
import random
random.seed(1)

# add walls first
dim = (15,15,15)
walls = aabbWalls(((0,0,0),(dim)))
wallIds = O.bodies.append(walls)

# load spheres from file, including information of their agglomerates ids
attrs = []
sp = ymport.textExt('/tmp/divided.txt',format='x_y_z_r_attrs',attrs=attrs)
n = max(int(a[0]) for a in attrs)+1
colors = [randomColor() for _ in xrange(n)]
agglomerates = [[] for _ in xrange(n)]
for s,a in zip(sp,attrs):
	aa = int(a[0])
	s.agglomerate = aa
	s.shape.color = colors[aa]
	agglomerates[aa].append(s)
for a in agglomerates:
	O.bodies.appendClumped(a)

O.engines = [
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	TriaxialStressController(
		thickness = 0,
		stressMask = 7,
		internalCompaction = False,
		label = 'compressor',
	),
	NewtonIntegrator(damping=.6),
]
O.dt = PWaveTimeStep()

# compress the sample
compressor.goal1 = compressor.goal2 = compressor.goal3 = -1e-7
O.run(50000,True)
compressor.goal1 = compressor.goal2 = compressor.goal3 = -1e-5
O.run(30000,True)

# save the result, including information of agglomerates which the particle belongs to
export.textExt('/tmp/compressed.txt','x_y_z_r_attrs',attrs=['b.agglomerate'])

try:
	from yade import qt
	qt.View()
except:
	pass
