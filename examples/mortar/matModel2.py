from yade import plot

dt = 1
young=1e9

x,y,z = .2,.5,.7

mortar = MortarMat(young=young)
polyMat = PolyhedraMat(young=young/z)
for mat in (mortar,polyMat):
	O.materials.append(mat)

O.dt = dt
bs = b1,b2 = [polyhedron(((-x,-y,-z),(+x,-y,-z),(-x,+y,-z),(+x,+y,-z),(-x,-y,+z),(+x,-y,+z),(-x,+y,+z),(+x,+y,+z)),material=mortar) for i in (0,1)]
b2.state.pos = b2.state.refPos = (0,0,2*z)
for b in bs:
	b.state.blockedDOFs = 'xyzXYZ'
O.bodies.append(bs)

def plotAddData():
	i = O.interactions[0,1]
	if i.phys:
		plot.addData(
			fn = i.phys.normalForce.norm(),
			dspl = O.bodies[1].state.displ().norm(),
		)

#
factor=1.1
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Polyhedra_Aabb(aabbEnlargeFactor=factor,label='bo1')]),
	InteractionLoop(
		[Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom(label='ig2')], 
		[Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys(),Ip2_MortarMat_MortarMat_MortarPhys()],
		[Law2_PolyhedraGeom_PolyhedraPhys_Volumetric(),Law2_ScGeom_MortarPhys_Lourenco()],
	),
	NewtonIntegrator(),
	PyRunner(iterPeriod=1,command='plotAddData()'),
	PyRunner(iterPeriod=1,command='print O.interactions[0,1].phys'),
]
ig2.ig2scGeom.interactionDetectionFactor = factor
O.step()
phys = O.interactions[0,1].phys
phys.kn = young*2*x*2*y/(2*z)
ig2.ig2scGeom.interactionDetectionFactor = bo1.aabbEnlargeFactor = 1
ig2.createScGeom=False
b2.state.vel = (0,0,-z*.002)
for b in O.bodies:
	b.mat = polyMat

plot.plots = {'dspl':'fn'}
plot.plot()
O.run(30)
