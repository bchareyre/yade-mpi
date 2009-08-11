O.periodicCell=(0,0,0),(20,20,20)
from yade import pack,log,timing
p=pack.SpherePack()
p.makeCloud(O.periodicCell[0],O.periodicCell[1],1,.5,700,True)
for sph in p:
	O.bodies.append(utils.sphere(sph[0],sph[1],density=1000,dynamic=True))

#log.setLevel("PeriIsoCompressor",log.TRACE)
O.timingEnabled=True
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),MetaInteractingGeometry2AABB()]),
	PeriodicInsertionSortCollider(),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	PeriIsoCompressor(charLen=.1,stresses=[50e9,1e8],doneHook="print 'FINISHED'; O.pause() "),
	NewtonsDampedLaw(damping=.4)
]
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt; qt.Controller(); qt.View()
O.run()
O.wait()
timing.stats()

# now take that packing and pad some larger volume with it
sp=pack.SpherePack()
sp.fromSimulation() # take spheres from simulation; cellSize is set as well
O.reset()
print sp.cellSize
sp.cellFill((30,30,30))
print sp.cellSize
for s in sp:
	O.bodies.append(utils.sphere(s[0],s[1]))
