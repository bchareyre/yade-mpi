from yade import plot

dt = 1e-5

mortar = MortarMat()
polyMat = PolyhedraMat()
for mat in (mortar,polyMat):
	O.materials.append(mat)

def sim(angle):
	O.reset()
	O.dt = dt
	bs = b1,b2 = [polyhedron(((-1,-1,-1),(+1,-1,-1),(-1,+1,-1),(+1,+1,-1),(-1,-1,+1),(+1,-1,+1),(-1,+1,+1),(+1,+1,+1)),material=mortar) for i in (0,1)]
	b2.state.pos = (0,0,2)
	for b in bs:
		b.state.blockedDOFs = 'xyzXYZ'
	O.bodies.append(bs)
	#
	factor=1.1
	O.engines=[
		ForceResetter(),
		InsertionSortCollider([Bo1_Polyhedra_Aabb(aabbEnlargeFactor=factor,label='bo1')]),
		InteractionLoop(
			[Ig2_Polyhedra_Polyhedra_PolyhedraGeomOrScGeom(label='ig2')], 
			[Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys(),Ip2_MortarMat_MortarMat_MortarPhys()],
			[Law2_PolyhedraGeom_PolyhedraPhys_Volumetric(),Law2_ScGeom_MortarPhys_Lourenco()]
		),
		NewtonIntegrator(),
	]
	ig2.ig2scGeom.interactionDetectionFactor = factor
	O.step()
	ig2.ig2scGeom.interactionDetectionFactor = bo1.aabbEnlargeFactor = 1
	b2.state.vel = (sin(angle),0,cos(angle))
	while len([i for i in O.interactions]):
		sn,st = i.phys.sigmaN, i.phys.sigmaT.norm()
		O.step()
		if O.iter > 1e6:
			raise RuntimeError, "TODO"
	plot.addData(sn=sn,st=st)

n = 50
for i in xrange(n):
	sim(i*pi/(n-1))

plot.plots = {'sn':'st'}
plot.matplotlib.pyplot.axes().set_aspect(1)
p = plot.plot()
