# -*- encoding=utf-8 -*-

#
# FIXME: ParallelEngine is not currently wrapped in python
#

O.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	]
O.engines=[
	# physical actions will not be needed until the contact law comes in;
	# therefore it can run in parallel with the Aabb engines and collider;
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
		# ForceResetter will run in parallel with the second group of BoundingVolumeMEtaEngine+PersistentSAPCollider
		ForceResetter(),
		# Engines within the group will be run serially, however
		[BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),	PersistentSAPCollider(),]
	]),
	InteractionGeometryDispatcher([Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()]),
	InteractionPhysicsDispatcher([Ip2_FrictMat_FrictMat_FrictPhys()]),
	# the rest must also be run sequentially
	# (contact law as well as gravity modify physical actions, which are, once computed, used in the integrator)
	ElasticContactLaw(),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=.2)
]

from yade import utils
O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
O.dt=.5*utils.PWaveTimeStep()
O.saveTmp()
#o.run(100000); o.wait(); print o.iter/o.realtime,"iterations/sec"
