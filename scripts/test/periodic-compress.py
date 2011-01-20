O.periodic=True
O.cell.refSize=(20,20,10)
from yade import pack,log,timing
O.materials.append(FrictMat(young=30e9,density=2400))
p=pack.SpherePack()
p.makeCloud(Vector3().ZERO,O.cell.refSize,1,.5,700,True)
for sph in p:
	O.bodies.append(utils.sphere(sph[0],sph[1]))

log.setLevel("PeriIsoCompressor",log.DEBUG)
O.timingEnabled=True
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
	),
	PeriIsoCompressor(charLen=.5,stresses=[-50e9,-1e8],doneHook="print 'FINISHED'; O.pause() ",keepProportions=True),
	NewtonIntegrator(damping=.4,homotheticCellResize=1)
]
O.dt=utils.PWaveTimeStep()
O.saveTmp()
print O.cell.refSize
from yade import qt; qt.Controller(); qt.View()
O.run()
O.wait()
timing.stats()
#while True:
#	O.step()


# now take that packing and pad some larger volume with it
#sp=pack.SpherePack()
#sp.fromSimulation() # take spheres from simulation; cellSize is set as well
#O.reset()
#print sp.cellSize
#sp.cellFill((30,30,30))
#print sp.cellSize
#for s in sp:
#	O.bodies.append(utils.sphere(s[0],s[1]))
