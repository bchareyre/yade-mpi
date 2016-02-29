#!/usr/bin/env python
# encoding: utf-8

# The model checks liquid migration model if it is enabled during compilation
from yade import utils, plot

if ('LIQMIGRATION' in features):
  o = Omega()
  fr = 0.5;rho=2000
  tc = 0.001; en = 0.7; et = 0.7; 
  o.dt = 1.0
  
  
  r1 = 1.0
  r2 = 1.0
  Gamma = 20.6*1e-3
  Theta = 0
  VB = 74.2*1e-12
  
  tolerance = 1e-6
  
  
  CapillarType = "Lambert"
  
  mat1 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType,tc=tc,en=en,et=et))
  
  d = 1.1
  id0 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True, color=[1,0,0]))
  
  id1 = O.bodies.append(sphere(center=[0,0,(r1+r2)*d],radius=r2,material=mat1,fixed=True, color=[0,1,0]))
  id2 = O.bodies.append(sphere(center=[0,0,-(r1+r2)*d],radius=r2,material=mat1,fixed=True, color=[0,1,0]))
  
  
  O.bodies[id0].state.Vf = 0.3e-1
  O.bodies[id0].state.Vmin = 0.1e-1
  
  O.bodies[id1].state.Vf = 0.4e-1
  O.bodies[id1].state.Vmin = 0.1e-1
  
  O.bodies[id2].state.Vf = 0.5e-1
  O.bodies[id2].state.Vmin = 0.1e-1
  
  vel = -0.15
  O.bodies[id1].state.vel=[0,0,vel]
  O.bodies[id2].state.vel=[0,0,-vel]
  
  o.engines = [
    ForceResetter(),
    InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=(r1+r2)*5.0),
    InteractionLoop(
      [Ig2_Sphere_Sphere_ScGeom()],
      [Ip2_ViscElCapMat_ViscElCapMat_ViscElCapPhys()],
      [Law2_ScGeom_ViscElCapPhys_Basic()],
    ),
    LiqControl(particleconserve=True,label='liqcontrol'),
    NewtonIntegrator(damping=0,gravity=[0,0,0]),
    PyRunner(command='showData()',iterPeriod=1,dead=True),
  ]
  
  def showData():
    print "Step %d"%O.iter
    print "idB=%d, Vf=%s, Vmin=%s;"%(id0, O.bodies[id0].state.Vf, O.bodies[id0].state.Vmin)
    print "idB=%d, Vf=%s, Vmin=%s;"%(id1, O.bodies[id1].state.Vf, O.bodies[id1].state.Vmin)
    print "idB=%d, Vf=%s, Vmin=%s;"%(id2, O.bodies[id2].state.Vf, O.bodies[id2].state.Vmin)
    try:
      print "Interaction[%d, %d].Vb=%s"%(id0, id1, O.interactions[id0,id1].phys.Vb)
    except:
      pass
    
    try:
      print "Interaction[%d, %d].Vb=%s"%(id0, id2, O.interactions[id0,id2].phys.Vb)
    except:
      pass
    print 
  
  def switchVel():
    O.bodies[id1].state.vel=-O.bodies[id1].state.vel
    O.bodies[id2].state.vel=-O.bodies[id2].state.vel
  
  resultStatus = 0
  O.run(3, True)
  if ((abs((O.interactions[id0,id1].phys.Vb - 0.03)/0.03) > tolerance) or 
      (abs((O.interactions[id0,id1].phys.Vb - 0.03)/0.03) > tolerance)):
    resultStatus += 1
  
  switchVel()
  O.run(5, True)
  if ((abs((O.bodies[id0].state.Vf - 0.03)/0.03) > tolerance) or 
      (abs((O.bodies[id1].state.Vf - 0.04)/0.04) > tolerance) or
      (abs((O.bodies[id2].state.Vf - 0.05)/0.05) > tolerance)):
    resultStatus += 1
  
  liqcontrol.particleconserve=False
  switchVel()
  O.run(5, True)
  switchVel()
  O.run(5, True)
  if ((abs((O.bodies[id0].state.Vf - 0.0465)/0.0465) > tolerance) or 
      (abs((O.bodies[id1].state.Vf - 0.0325)/0.0325) > tolerance) or
      (abs((O.bodies[id2].state.Vf - 0.041)/0.041) > tolerance)):
    resultStatus += 1
else:
  print "This checkLiquidMigration.py cannot be executed because LIQMIGRATION is disabled"
  
