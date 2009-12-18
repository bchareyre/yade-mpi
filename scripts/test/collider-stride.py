#
# chec kvisually whether swept bounding boxes work as expected
#

O.bodies.append([
	utils.facet([[1,0,0],[-1,2,0],[-1,-2,0]]),
	utils.sphere([0,0,2],.5,color=(0,1,0),density=2e3,velocity=[0,0,-1])
])
O.dt=utils.PWaveTimeStep()

O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(label='isc'),
	InteractionDispatchers(
		[ef2_Facet_Sphere_Dem3DofGeom(),ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	GravityEngine(gravity=[0,0,-100]),
	NewtonIntegrator(damping=0)
]

from yade import timing,qt,log
try:
	renderer=qt.Renderer()
	renderer['Body_bounding_volume']=True
	qt.Controller(); qt.View()
except ImportError: pass

O.timingEnabled=True
isc['sweepLength']=.1
isc['nBins']=5
#log.setLevel('InsertionSortCollider',log.DEBUG)
#log.setLevel('BoundDispatcher',log.DEBUG)
O.saveTmp()
O.step()
