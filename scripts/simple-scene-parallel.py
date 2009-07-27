# -*- encoding=utf-8 -*-

o=Omega()

o.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
o.engines=[
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
		StandAloneEngine('PhysicalActionContainerReseter'),
		[
			MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')]),
			StandAloneEngine('PersistentSAPCollider'),
		]
	]),
	# interaction physics depends on interaction geometry
	MetaEngine('InteractionGeometryMetaEngine',[EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	# the rest must also be run sequentially
	# (contact law as well as gravity modify physical actions, which are, once computed, used in the integrator)
	StandAloneEngine('ElasticContactLaw'),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-9.81]}),
	NewtonsDampedLaw(damping=.2)
]

from yade import utils
o.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
o.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
o.dt=.5*utils.PWaveTimeStep()

#o.run(100000); o.wait(); print o.iter/o.realtime,"iterations/sec"
