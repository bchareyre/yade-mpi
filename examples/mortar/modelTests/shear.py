from yade import plot
######################################################################
# Script to test brick material in shear. The material is
# initially BrickMat. After failure and deletion of cohesive mortar
# interaction, the simulation continues with PolyhedraMat.
######################################################################

######################################################################
# I N P U T S
######################################################################
x,y,z = .2,.5,.7 # dimension of the bricks
vDspl = -1e-3 # vertical displacement (+=tension, -=compression)
hDspl = 1e-3 # horizontal displacement for shear
nSteps = 1000 # each phase will take nSteps iterations
######################################################################
young = 4e9
poisson = .2
tensileStrength = .2e3
cohesion = .3e6
frictionAngle = atan(.5)
compressiveStrength = 10e6
frictionAngle2 = atan(.4) # independent friction angle for PolyhedraMat for residual strength
######################################################################

O.dt = 1e-3

mortar = MortarMat(young=young,poisson=1/(2*(1+poisson)),tensileStrength=tensileStrength,cohesion=cohesion,frictionAngle=frictionAngle,compressiveStrength=compressiveStrength)
polyMat = PolyhedraMat(young=young/z,poisson=1e3,frictionAngle=frictionAngle2)
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
	dspl = O.bodies[1].state.displ()
	dn = dspl[2]
	ds = dspl[0]
	try:
		i = O.interactions[0,1]
		fn = i.phys.normalForce.norm()
		fs = i.phys.shearForce.norm()
	except (IndexError,AttributeError):
		fn = fs = 0.
	plot.addData(
		i = O.iter,
		dn = dn,
		ds = ds,
		fn = fn,
		fs = fs,
	)

# checkpoints
def checkpoint():
	if O.iter==nSteps:
		O.bodies[1].state.vel = (hDspl/(nSteps*O.dt),0,0)
	elif O.iter==2*nSteps:
		O.pause()
		plot.saveDataTxt('/tmp/shear.dat')

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
	PyRunner(iterPeriod=1,command='checkpoint()'),
]
ig2.ig2scGeom.interactionDetectionFactor = factor
O.step()
# after O.step, a cohesive interaction is created and we can change material of the bodies. It has no effect on existing interactions, but newly created interactions will be created form the new material
for b in O.bodies:
	b.mat = polyMat
ig2.createScGeom = False # also for new interactions, create PolyhedraGeom rather than ScGeom
ig2.ig2scGeom.interactionDetectionFactor = bo1.aabbEnlargeFactor = 1 # deactivate interaction detection enlargement
phys = O.interactions[0,1].phys
phys.kn = young*2*x*2*y/(2*z) # sets correct kn of interaction
phys.crossSection = 2*x*2*y

plot.plots = {'dn':'fn','ds':'fs','i':('fn','fs')}
plot.plot()
b2.state.vel = (0,0,vDspl/(nSteps*O.dt))

O.run()
