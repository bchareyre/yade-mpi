O.cellSize=Vector3(.5,.5,.5)
O.bodies.append(utils.facet([[.4,.0001,.3],[.2,.0001,.3],[.3,.2,.2]]))
O.bodies.append(utils.sphere([.3,.1,.4],.05,dynamic=True))
O.bodies.append(utils.sphere([.200001,.2000001,.4],.05,dynamic=False))
O.bodies.append(utils.sphere([.3,0,0],.1,dynamic=False))
O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()]
	),
	GravityEngine(gravity=(0,0,-10)),
	NewtonIntegrator(),
]

g=0.
while False:
	O.cellShear=Vector3(.2*sin(g),.2*cos(pi*g),.2*sin(2*g)+.2*cos(3*g))
	time.sleep(0.001)
	g+=1e-3
O.cellShear=(.15,.05,.2)
O.dt=2e-2*utils.PWaveTimeStep()
O.step()
O.saveTmp()
rdr=yade.qt.Renderer()
#rdr['Body_bounding_volume']=True
rdr['intrAllWire']=True
#from yade import log
#import yade.qt,time
v=yade.qt.View()
v.axes=True
v.grid=(True,True,True)
