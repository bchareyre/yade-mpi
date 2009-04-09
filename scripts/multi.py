#
# see http://yade.wikia.com/wiki/ScriptParametricStudy#scripts.2Fmulti.py for explanations
#
## read parameters from table here
from yade import utils
utils.readParamsFromTable(gravity=-9.81,density=2400,initialSpeed=20,noTableOk=True)
print gravity,density,initialSpeed

o=Omega()
o.initializers=[
		BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()])
	]
o.engines=[
	PhysicalActionContainerReseter(),
	BoundingVolumeMetaEngine([
		InteractingSphere2AABB(),
		InteractingBox2AABB(),
		MetaInteractingGeometry2AABB()
	]),
	PersistentSAPCollider(),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(),InteractingBox2InteractingSphere4SpheresContactGeometry()],
		[SimpleElasticRelationships(),],
		[ef2_Spheres_Elastic_ElasticLaw(),]
	)
	GravityEngine(gravity=(0,0,gravity)), ## here we use the 'gravity' parameter
	NewtonsDampedLaw(damping=0.4)
]
o.bodies.append(utils.box([0,50,0],extents=[1,50,1],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=density)) ## here we use the density parameter
o.bodies.append(utils.sphere([0,0,10],1,color=[0,1,0],young=30e9,poisson=.3,density=density)) ## here again

o.bodies[1].phys['velocity']=[0,initialSpeed,0] ## assign initial velocity

o.dt=.8*utils.PWaveTimeStep()
## o.saveTmp('initial')

# run 30000 iterations
o.run(30000,True)

# write some results to a common file
# (we rely on the fact that 2 processes will not write results at exactly the same time)
# 'a' means to open for appending
file('multi.out','a').write('%s %g %g %g %g\n'%(o.tags['description'],gravity,density,initialSpeed,o.bodies[1].phys.pos[1]))
