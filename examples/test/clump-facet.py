O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_Facet_Aabb(),
	]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(),
]

s1 = sphere((2,2,0),1)
s2 = sphere((-2,2,0),1,fixed=True)
f1 = facet(((0,0,-2),(0,0,2),(+5,0,0)),wire=False)
f2 = facet(((0,0,-2),(0,0,2),(-5,0,0)),wire=False)
# needs to assign (any) nonzero mass and inertia to facets. Total mass and inertia of clump itself can be assigned independently
f1.state.mass = f2.state.mass = 1
f2.state.inertia = f2.state.inertia = (1,1,1)
O.bodies.append((s1,s2))
clumpId,facetsId = O.bodies.appendClumped((f1,f2))
s = O.bodies[clumpId].state
# now we can assign arbitrary inertia of the overall clump
s.mass = 5000
s.inertia = 10000*Vector3(1,1,1)
s.blockedDOFs = 'xyzXY'
O.forces.setPermT(clumpId,(0,0,1e1))

O.dt = 5e-5
try:
	from yade import qt
	qt.View()
except:
	pass	
