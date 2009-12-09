O.initializers=[BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()])]
O.engines=[
	PhysicalActionContainerReseter(),
	BoundDispatcher([
		InteractingSphere2AABB(),
		InteractingBox2AABB(),
		MetaInteractingGeometry2AABB()
	]),
	PersistentSAPCollider(),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(),InteractingBox2InteractingSphere4SpheresContactGeometry()],
		[SimpleElasticRelationships()],
		[ef2_Spheres_Elastic_ElasticLaw()]
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonIntegrator(damping=.2)
]
# support
O.bodies.append(utils.box([0,0,-1.5],[3,3,.2],dynamic=False))
# stand-alone sphere
O.bodies.append(utils.sphere([0,0,0],.5,density=1000,color=[1,1,1]))
# clumps
relPos=[(0,-.5,-.5),(0,.5,0),(.5,0,0),(0,0,.5)]
coords=[(-2,0,0),(2,0,0),(0,2,0),(0,-2,0)]
for i,cc in enumerate(coords):
	# This shorthand command does something like this:
	# O.bodies.appendClumped([utils.sphere(...),utils.sphere(...),utils.sphere(...)])
	# and returns tuple of clumpId,[bodyId1,bodyId2,bodyId3]
	clump,spheres=O.bodies.appendClumped([utils.sphere([relPos[j][0]+coords[i][0],relPos[j][1]+coords[i][1],relPos[j][2]+coords[i][2]],.5,density=1000) for j in range(0,i+1)])
	print clump,spheres
O.dt=utils.PWaveTimeStep()
O.saveTmp('init')

