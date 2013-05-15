'''
This example shows compression of a packing with a periodic cell.
'''

O.periodic=True
O.cell.setBox(20,20,10)

from yade import pack,timing

O.materials.append(FrictMat(young=30e9,density=2400))
p=pack.SpherePack()
p.makeCloud(Vector3(0,0,0),Vector3(20,20,10),1,.5,700,True)
for sph in p:
	O.bodies.append(sphere(sph[0],sph[1]))


O.timingEnabled=True
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],allowBiggerThanPeriod=True),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	PeriIsoCompressor(charLen=.5,stresses=[-50e9,-1e8],doneHook="print 'FINISHED'; O.pause() ",keepProportions=True),
	NewtonIntegrator(damping=.4)
]
O.dt=PWaveTimeStep()
O.saveTmp()
#print O.cell.refSize
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
#	O.bodies.append(sphere(s[0],s[1]))
