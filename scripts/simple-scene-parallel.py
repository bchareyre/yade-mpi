# -*- encoding=utf-8 -*-

#
# FIXME: ParallelEngine is not currently wrapped in python
#

O.initializers=[
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()]),
	]
O.engines=[
	# physical actions will not be needed until the contact law comes in;
	# therefore it can run in parallel with the AABB engines and collider;
	# but since collider depends on AABBs, they will run sequentially, one after another
	#
	# ParallelEngine takes list of engines, which are run in parallel;
	# 	if an element of the list is itself a list, it is run sequentially.
	#	openMP has number of environment variables (notably, how many threads to use):
	#  	http://docs.sun.com/source/819-0501/5_compiling.html#19273
	#
	# 	try OMP_NUM_THREAD=number yade-version simple-scene-parallel.py to see the difference
	#  by default, openMP creates as many threads as you have processor cores in your machine
	#  
	#  overview: http://en.wikipedia.org/wiki/Openmp
	#	homepage: http://openmp.org for details,
	#  implementation of openMP in gcc: http://gcc.gnu.org/projects/gomp/
	#
	ParallelEngine([
		# BexResetter will run in parallel with the second group of BoundingVolumeMEtaEngine+PersistentSAPCollider
		BexResetter(),
		# Engines within the group will be run serially, however
		[BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()]),	PersistentSAPCollider(),]
	]),
	InteractionGeometryDispatcher([InteractingSphere2InteractingSphere4SpheresContactGeometry(),InteractingBox2InteractingSphere4SpheresContactGeometry()]),
	InteractionPhysicsDispatcher([SimpleElasticRelationships()]),
	# the rest must also be run sequentially
	# (contact law as well as gravity modify physical actions, which are, once computed, used in the integrator)
	ElasticContactLaw(),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonsDampedLaw(damping=.2)
]

from yade import utils
O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
O.dt=.5*utils.PWaveTimeStep()
O.saveTmp()
#o.run(100000); o.wait(); print o.iter/o.realtime,"iterations/sec"
