"Script showing shear interaction between facet/wall and sphere."
O.bodies.append([
	#utils.sphere([0,0,0],1,dynamic=False,color=(0,1,0),wire=True),
	utils.facet(([2,2,1],[-2,0,1],[2,-2,1]),dynamic=False,color=(0,1,0),wire=False),
	#utils.wall([0,0,1],axis=2,color=(0,1,0)),
	utils.sphere([-1,0,2],1,dynamic=True,color=(1,0,0),wire=True),
])
O.engines=[
	BoundingVolumeMetaEngine([
		InteractingSphere2AABB(),InteractingFacet2AABB(),Wall2AABB()
	]),
	InsertionSortCollider(),
	InteractionGeometryMetaEngine([
		ef2_Sphere_Sphere_Dem3DofGeom(),
		ef2_Facet_Sphere_Dem3DofGeom(),
		ef2_Wall_Sphere_Dem3DofGeom()
	]),
	#GravityEngine(gravity=(0,0,-10))
	RotationEngine(rotationAxis=[0,1,0],angularVelocity=10,subscribedBodies=[1]),
	TranslationEngine(translationAxis=[1,0,0],velocity=10,subscribedBodies=[1]),
	#NewtonsDampedLaw()
]
O.miscParams=[
	GLDraw_Dem3DofGeom_SphereSphere(normal=True,rolledPoints=True,unrolledPoints=True,shear=True,shearLabel=True),
	Gl1_Dem3DofGeom_FacetSphere(normal=False,rolledPoints=True,unrolledPoints=True,shear=True,shearLabel=True),
	Gl1_Dem3DofGeom_WallSphere(normal=False,rolledPoints=True,unrolledPoints=True,shear=True,shearLabel=True),
	GLDrawInteractingSphere(wire=True)
]

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Body_wire']=True
	renderer['Body_geometrical_model']=False
	renderer['Body_interacting_geom']=True
	renderer['Interaction_geometry']=True
	qt.Controller()
	qt.View()
except ImportError: pass

O.dt=1e-6
O.saveTmp('init')
O.run(1)
