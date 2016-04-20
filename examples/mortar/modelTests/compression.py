from yade import plot
######################################################################
# Script to test brick material in compression. The material is
# initially BrickMat. After failure and deletion of cohesive mortar
# interaction, the simulation continues with PolyhedraMat.
######################################################################

O.dt = 1
young=1e9

x,y,z = .2,.5,.7 # dimension of the bricks

mortar = MortarMat(young=young)
polyMat = PolyhedraMat(young=young/z) # stiffness of polyhedra law relates force and interactiong volume, to have the same stiffness, young has to be divided by z
for mat in (mortar,polyMat):
	O.materials.append(mat)

# two bricks with blockedDOFs, initially with MortarMat
bs = b1,b2 = [polyhedron(((-x,-y,-z),(+x,-y,-z),(-x,+y,-z),(+x,+y,-z),(-x,-y,+z),(+x,-y,+z),(-x,+y,+z),(+x,+y,+z)),material=mortar) for i in (0,1)]
b2.state.pos = b2.state.refPos = (0,0,2*z)
for b in bs:
	b.state.blockedDOFs = 'xyzXYZ'
O.bodies.append(bs)

# function to plot the results
def plotAddData():
	i = O.interactions[0,1]
	if i.phys:
		plot.addData(
			fn = i.phys.normalForce.norm(),
			dspl = O.bodies[1].state.displ().norm(),
		)

#
# factor to safely create interaction of just touching bricks
factor=1.1
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Polyhedra_Aabb(aabbEnlargeFactor=factor,label='bo1')]),
	InteractionLoop(
		# both ScGeom and PolyhedraGeom, MortarMat and PolyhedraMat
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
# after O.step, a cohesive interaction is created and we can change material of the bodies. It has no effect on existing interactions, but newly created interactions will be created form the new material
for b in O.bodies:
	b.mat = polyMat
ig2.createScGeom = False # also for new interactions, create PolyhedraGeom rather than ScGeom
ig2.ig2scGeom.interactionDetectionFactor = bo1.aabbEnlargeFactor = 1 # deactivate interaction detection enlargement
O.interactions[0,1].phys.kn = young*2*x*2*y/(2*z) # sets correct kn of interaction

plot.plots = {'dspl':'fn'}
plot.plot()
b2.state.vel = (0,0,-z*.002)
O.run(30)
