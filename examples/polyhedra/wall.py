from yade import qt
import numpy as np
import random
from yade import polyhedra_utils

m = PolyhedraMat()
m.density = 1000  
m.young = 5E8 
m.poisson = 5E6/5E8
m.frictionAngle = 0.7 

size = 0.1;
vertices = [[0,0,0],[size,0,0],[size,size,0],[size,size,size],[0,size,0],[0,size,size],[0,0,size],[size,0,size]]

for i in range(0,10):	
	for j in range(0,10):
		t = polyhedra_utils.polyhedra(m,v=vertices)
		t.state.pos = (0,(i+0.5)*size-5*size,(j+0.5)*size*1)
		O.bodies.append(t)
	
	

qt.Controller()
V = qt.View()
V.screenSize = (750,550)
V.sceneRadius = 1
V.eyePosition = (-1.5,1.5,0.5)
V.lookAt = (0,-0.5,0)
V.upVector = (0,0,1)


O.bodies.append(utils.wall(0,axis=2,sense=1,material=m))

def checkUnbalanced():
   print ('iter %d, time elapsed %f, unbalanced forces = %f'%(O.iter, O.realtime, utils.unbalancedForce()))
   if O.iter<200: return
   if utils.unbalancedForce()>0.01: return 
   AddBall()	

def AddBall():
	ball = polyhedra_utils.polyhedralBall(size*1, 40, m, (-size*2.5,0,size*7),mask=1)
	ball.shape.color = (0,0,0.9)
	ball.state.vel = (15,0,0)
	O.bodies.append(ball)
	checker.dead = True

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb()]),
   InteractionLoop(
      [Ig2_Wall_Polyhedra_PolyhedraGeom(), Ig2_Polyhedra_Polyhedra_PolyhedraGeom(), Ig2_Facet_Polyhedra_PolyhedraGeom()], 
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()], # collision "physics"
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]   # contact law -- apply forces
   ),
   #GravityEngine(gravity=(0,0,-9.81)),
   NewtonIntegrator(damping=0.5,gravity=(0,0,-9.81)),
   PyRunner(command='checkUnbalanced()',realPeriod=3,label='checker')#, 	
   # wideo_recording
   #qt.SnapshotEngine(fileBase='W',iterPeriod=50,label='snapshot')
]

#O.dt=.5*polyhedra_utils.PWaveTimeStep()
O.dt = 0.000025
O.saveTmp()

#O.run()


#comment ball to see stability 



