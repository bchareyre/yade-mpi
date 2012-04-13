""" THIS SCRIPT IS NOT WORKING!
ERROR MESSAGE:

Running script periodic-compress.py                                                                                                  
787   WARN  yade.SpherePack /home/me/YADE/yade/pkg/dem/SpherePack.cpp:212 makeCloud: Exceeded 1000 tries to insert non-overlapping sphere to packing. Only 106 spheres was added, although you requested 700.                                                             
Vector3(20,20,10)                                                                                                                    
1567  FATAL yade.InsertionSortCollider /home/me/YADE/yade/pkg/common/InsertionSortCollider.cpp:458 spatialOverlapPeri: Body #15 spans over half of the cell size 6.93 (axis=2, min=4.26332, max=0.810043, span=10.3833, see flag allowBiggerThanPeriod)                   
1567  FATAL yade.ThreadRunner /home/me/YADE/yade/core/ThreadRunner.cpp:31 run: Exception occured:                                    
/home/me/YADE/yade/pkg/common/InsertionSortCollider.cpp: Body larger than half of the cell size encountered.                         
1581  ERROR yade.qt4   py/wrapper/yadeWrapper.cpp:372 wait: Simulation error encountered.                                            
Traceback (most recent call last):                                                                                                   
  File "/home/me/YADE/YADE3041/bin/yade-bzr3041", line 182, in runScript                                                             
    execfile(script,globals())                                                                                                       
  File "periodic-compress.py", line 28, in <module>                                                                                  
    O.wait()                                                                                                                         
RuntimeError: std::exception 
"""

O.periodic=True
O.cell.setBox(20,20,10)
from yade import pack,log,timing
O.materials.append(FrictMat(young=30e9,density=2400))
p=pack.SpherePack()
p.makeCloud(Vector3(0,0,0),Vector3(20,20,10),1,.5,700,True)
for sph in p:
	O.bodies.append(utils.sphere(sph[0],sph[1]))


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
	NewtonIntegrator(damping=.4)
]
O.dt=utils.PWaveTimeStep()
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
#	O.bodies.append(utils.sphere(s[0],s[1]))
