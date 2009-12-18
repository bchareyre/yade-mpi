o=Omega()

o.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers([ef2_Facet_Sphere_Dem3DofGeom()],[SimpleElasticRelationships()],[Law2_Dem3Dof_Elastic_Elastic()],),
	GravityEngine(gravity=[0,0,-10]),
	NewtonIntegrator(damping=0.01),
]

O.bodies.append([
	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],dynamic=False,color=[1,0,0],young=1e3),
	utils.facet([[1,-1,0],[0,1,0,],[1,.5,.5]],dynamic=False,young=1e3)
])
import random
if 1:
	for i in range(0,100):
		O.bodies.append(utils.sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05),velocity=[random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1)]))
else:
	O.bodies.append(utils.sphere([0,0,.6],.5))
O.dt=1e-4
O.saveTmp('init')
import yade.log
#yade.log.setLevel("InsertionSortCollider",yade.log.TRACE);
# compare 2 colliders:
if 0:
	O.timingEnabled=True
	from yade import timing
	for collider in InsertionSortCollider(),PersistentSAPCollider(haveDistantTransient=True):
		for i in range(2):
			O.loadTmp('init')
			utils.replaceCollider(collider)
			O.run(100,True)
			timing.reset()
			O.run(50000,True)
			timing.stats()
else:
	#O.run(100,True)
	O.step()
	print len(O.interactions)
	#O.bodies[2].phys['se3']=[-.6,0,.6,1,0,0,0]
	#O.step()
