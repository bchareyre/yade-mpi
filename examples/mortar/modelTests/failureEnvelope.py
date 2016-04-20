from yade import plot
######################################################################
# a script to test failure envelope of mortar material. Several
# simulations with one interaction are run until the stresses reach
# failure condition. The interaction is then deleted.
######################################################################

######################################################################
young = 3e9
GOverE = .2
tensileStrength = .2e6
cohesion = .3e6
frictionAngle = atan(.5)
compressiveStrength = 10e6
ellAspect = 3
######################################################################

# function to run monothonically increasing dformation in given angle. 0=tension, pi=compression, pi/2=shear, (0;pi/2)=tension+shear, (pi/2;pi)=compression+shear
def sim(angle):
	O.reset()
	O.dt = 1e-5
	# create two bricks with all blockedDOFs
	mortar = O.materials.append(MortarMat(young=young,poisson=GOverE,tensileStrength=tensileStrength,cohesion=cohesion,frictionAngle=frictionAngle,compressiveStrength=compressiveStrength,ellAspect=ellAspect))
	bs = b1,b2 = [polyhedron(((-1,-1,-1),(+1,-1,-1),(-1,+1,-1),(+1,+1,-1),(-1,-1,+1),(+1,-1,+1),(-1,+1,+1),(+1,+1,+1)),material=mortar) for i in (0,1)]
	b2.state.pos = (0,0,2)
	for b in bs:
		b.state.blockedDOFs = 'xyzXYZ'
	O.bodies.append(bs)
	#
	# factor to safely create interaction of just touching bricks
	factor=1.1
	O.engines=[
		ForceResetter(),
		InsertionSortCollider([Bo1_Polyhedra_Aabb(aabbEnlargeFactor=factor,label='bo1')]),
		InteractionLoop(
			[Ig2_Polyhedra_Polyhedra_ScGeom(interactionDetectionFactor=factor,label='ig2')], 
			[Ip2_MortarMat_MortarMat_MortarPhys()],
			[Law2_ScGeom_MortarPhys_Lourenco()]
		),
		NewtonIntegrator(),
	]
	O.step()
	ig2.interactionDetectionFactor = bo1.aabbEnlargeFactor = 1 # reset the interaction detection enlargement
	b2.state.vel = (sin(angle),0,cos(angle)) # sets velocity to produce desired 
	while len([i for i in O.interactions]) > 0: # run simulatinon until the interaction is broken
		sn,st = i.phys.sigmaN, i.phys.sigmaT.norm() # store last values
		O.step()
		if O.iter > 1e6:
			raise RuntimeError, "TODO" # not to run forever
	plot.addData(sn=sn,st=st) # after the interaction is broken, save stress to plot.data and return
	return

# run n simulations from tension to compression
n = 50
for i in xrange(n):
	sim(i*pi/(n-1))

# plot the results
plot.plots = {'sn':'st'}
plot.matplotlib.pyplot.axes().set_aspect(1)
plot.plot()
