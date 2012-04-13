"""Script that shrinks the periodic cell progressively.
It prints strain and average stress (computed from total volume force)
once in a while."""

""" THIS SCRIPT IS NOT WORKING!
ERROR MESSAGE:

Running script periodic-grow.py                                                                                                      
strain -1.42108547152e-15 avg. stress  67.6779436711 unbalanced  0.032                                                               
strain -1.06581410364e-15 avg. stress  40387.4675162 unbalanced  0.149871627041                                                      
strain -3.37507799486e-15 avg. stress  3347256.0529 unbalanced  0.000614664073268                                                    
strain -1.06581410364e-15 avg. stress  9443091.31807 unbalanced  0.000315645813894                                                   
strain -4.4408920985e-15 avg. stress  18840771.7584 unbalanced  0.00027858810343                                                     
12088 FATAL yade.InsertionSortCollider /home/me/YADE/yade/pkg/common/InsertionSortCollider.cpp:458 spatialOverlapPeri: Body #133 spans over half of the cell size 6.11395 (axis=1, min=1.13615, max=-1.9046, span=9.1547, see flag allowBiggerThanPeriod)                 
12088 FATAL yade.ThreadRunner /home/me/YADE/yade/core/ThreadRunner.cpp:31 run: Exception occured:                                    
/home/me/YADE/yade/pkg/common/InsertionSortCollider.cpp: Body larger than half of the cell size encountered.                         
12108 ERROR yade.qt4   py/wrapper/yadeWrapper.cpp:372 wait: Simulation error encountered.                                            
Traceback (most recent call last):                                                                                                   
  File "/home/me/YADE/YADE3041/bin/yade-bzr3041", line 182, in runScript                                                             
    execfile(script,globals())                                                                                                       
  File "periodic-grow.py", line 31, in <module>                                                                                      
    O.run(2000,True)                                                                                                                 
RuntimeError: std::exception  
"""

from yade import timing
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.6)
]
import random
for i in xrange(250):
	O.bodies.append(utils.sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.5+random.random()))
cubeSize=20
# absolute positioning of the cell is not important
O.periodic=True
O.cell.setBox(cubeSize,cubeSize,cubeSize)
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
O.run(200,True)
rate=-1e-3*cubeSize/(O.dt*200)*Matrix3.Identity
O.cell.velGrad=rate
for i in range(0,25):
	O.run(2000,True)
	F,stiff=utils.totalForceInVolume()
	dim=O.cell.refSize; A=Vector3(dim[1]*dim[2],dim[0]*dim[2],dim[0]*dim[1])
	avgStress=sum([F[i]/A[i] for i in 0,1,2])/3.
	print 'strain',(cubeSize-dim[0])/cubeSize,'avg. stress ',avgStress,'unbalanced ',utils.unbalancedForce()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
