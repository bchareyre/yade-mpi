m = PolyhedraMat()
m.density = 2600 #kg/m^3 
m.Ks = 20000
m.Kn = 1E9 #Pa
m.frictionAngle = 0.6 #rad
O.materials.append(m)

######################################################################
# basic test
t1 = polyhedron(((0,0,0),(0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)))
t2 = polyhedron(((0,0,1),(0,0,2),(0,1,1),(0,1,2),(2,0,1),(2,0,2),(2,1,1),(2,1,2)))

t1.state.mass = 1e-12
t1.state.inertia = (1e-12,1e-12,1e-12)
t1.state.pos = (0,.5,0)

t2.state.ori = ((1,2,3),1)
t2.state.pos = (sqrt(2),.5,-sqrt(2))

clumpId,polys = O.bodies.appendClumped((t1,t2))
clump = O.bodies[clumpId]
s = clump.state
s2 = t2.state
# if t1.mass and inertia are set to almost zero, clump properties should equal t2 properties
print s.pos, s2.pos
print s.mass, s2.mass
print s.inertia, s2.inertia
######################################################################


######################################################################
# something more real
wire = False
O.bodies.clear()
t1 = polyhedron(((0,0,0),(0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)),color=(0,1,0),wire=wire)
t2 = polyhedron(((0,0,1),(0,0,2),(0,1,1),(0,1,2),(2,0,1),(2,0,2),(2,1,1),(2,1,2)),color=(0,1,0),wire=wire)
bottom = polyhedron(((-5,-5,0),(5,-5,0),(5,5,0),(-5,5,0),(-5,-5,-1),(5,-5,-1),(5,5,-1),(-5,5,-1)),fixed=True,color=(0,1,1),wire=wire)
O.bodies.append(bottom)
clumpId,polys = O.bodies.appendClumped((t1,t2))
s = O.bodies[clumpId].state
s.pos = (0,0,5)
s.ori = Quaternion((1,1,.3),1.5)

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb(),Bo1_Facet_Aabb()]),
   InteractionLoop(
      [Ig2_Polyhedra_Polyhedra_PolyhedraGeom()],
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()],
   ),
   NewtonIntegrator(label='newton'),
]

O.dt=0.00001

try:
	from yade import qt
	qt.View()
except:
	pass
O.step()
newton.gravity = (0,0,-9.81)
######################################################################
