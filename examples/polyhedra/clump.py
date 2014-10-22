
def clumpPolyhedra(polyhedra):
	clumpId,junk = O.bodies.appendClumped(polyhedra)
	return
	clump = O.bodies[clumpId]
	clump.state.pos = sum((t.state.pos*t.state.mass for t in polyhedra),Vector3.Zero)/sum(t.state.mass for t in polyhedra)
	print clump.state.pos
	clump.state.mass = sum(t.state.mass for t in polyhedra)
	clump.state.inertia = sum((t.state.inertia for t in polyhedra),Vector3.Zero)	

m = PolyhedraMat()
m.density = 2600 #kg/m^3 
m.Ks = 20000
m.Kn = 1E6 #Pa
m.frictionAngle = 0.6 #rad
O.materials.append(m)

t1 = polyhedron(((0,0,0),(0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)))
t2 = polyhedron(((0,0,1),(0,0,2),(0,1,1),(0,1,2),(2,0,1),(2,0,2),(2,1,1),(2,1,2)))

t1.state.mass = 1e-12
t1.state.inertia = (1e-12,1e-12,1e-12)
t1.state.pos = (0,.5,0)

t2.state.ori = ((1,2,3),1)
t2.state.pos = (sqrt(2),.5,-sqrt(2))

clumpPolyhedra((t1,t2))


O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb(),Bo1_Facet_Aabb()]),
   InteractionLoop(
      [Ig2_Polyhedra_Polyhedra_PolyhedraGeom()],
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
   ),
   NewtonIntegrator()
]

O.dt=0.00025
#O.step()

try:
	from yade import qt
	qt.View()
except:
	pass
