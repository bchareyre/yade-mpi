from yade import polyhedra_utils

gravel1 = PolyhedraMat()
gravel1.IsSplitable = True
gravel1.strength = 1e0
gravel2 = PolyhedraMat()
gravel2.IsSplitable = True
gravel2.strength = 2e0
gravel3 = PolyhedraMat()
gravel3.IsSplitable = True
gravel3.strength = 4e0

steel = PolyhedraMat()
steel.young = 1e10

d = .05
p1 = polyhedra_utils.polyhedra(gravel1, size=(d,d,d), seed=1)
p2 = polyhedra_utils.polyhedra(gravel2, size=(d,d,d), seed=1)
p3 = polyhedra_utils.polyhedra(gravel3, size=(d,d,d), seed=1)
p2.state.pos = (2*d,0,0)
p3.state.pos = (4*d,0,0)
p2.state.ori = p3.state.ori = p1.state.ori

d = .035
w1 = utils.wall(+d, axis=1, sense=-1, material=steel)
w2 = utils.wall(-d, axis=1, sense=+1, material=steel)
v = 5e-1
w1.state.vel = (0,-v,0)
w2.state.vel = (0,+v,0)
O.bodies.append((p1,p2,p3,w1,w2))

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb()]),
   InteractionLoop(
      [Ig2_Wall_Polyhedra_PolyhedraGeom(), Ig2_Polyhedra_Polyhedra_PolyhedraGeom()], 
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
   ),
   NewtonIntegrator(),
	PolyhedraSplitter(iterPeriod=100),
]

O.dt=1e-6

try:
	from yade import qt
	qt.Controller()
	v = qt.View()
	v.ortho = True
except:
	pass

O.run(30000)
