#!/usr/bin/env python
# encoding: utf-8

# The simulation of different capillary bridge models.
# Parameters are set like in experiments of [Willett2000]

from yade import utils, plot
o = Omega()
fr = 0.5;rho=2000
tc = 0.001; en = 0.7; et = 0.7; o.dt = 0.0002*tc


r = 0.002381

Gamma = 20.6*1e-3
Theta = 0
VB = 74.2*1e-12


CapillarType1 = "Willett_numeric"
CapillarType2 = "Willett_analytic"
CapillarType3 = "Rabinovich"
CapillarType4 = "Lambert"
CapillarType5 = "Weigert"
CapillarType6 = "Soulie"


mat1 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType1,tc=tc,en=en,et=et))
mat2 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType2,tc=tc,en=en,et=et))
mat3 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType3,tc=tc,en=en,et=et))
mat4 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType4,tc=tc,en=en,et=et))
mat5 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType5,tc=tc,en=en,et=et))
mat6 = O.materials.append(ViscElCapMat(frictionAngle=fr,density=rho,Vb=VB,gamma=Gamma,theta=Theta,Capillar=True,CapillarType=CapillarType6,tc=tc,en=en,et=et))


id1 = O.bodies.append(sphere(center=[0,0,0],radius=r,material=mat1,fixed=True))
id2 = O.bodies.append(sphere(center=[0,0,2*r],radius=r,material=mat1,fixed=True))

id3 = O.bodies.append(sphere(center=[3.0*r,0,0],radius=r,material=mat2,fixed=True))
id4 = O.bodies.append(sphere(center=[3.0*r,0,2*r],radius=r,material=mat2,fixed=True))

id5 = O.bodies.append(sphere(center=[6.0*r,0,0],radius=r,material=mat3,fixed=True))
id6 = O.bodies.append(sphere(center=[6.0*r,0,2*r],radius=r,material=mat3,fixed=True))

id7 = O.bodies.append(sphere(center=[9.0*r,0,0],radius=r,material=mat4,fixed=True))
id8 = O.bodies.append(sphere(center=[9.0*r,0,2*r],radius=r,material=mat4,fixed=True))

id9 = O.bodies.append(sphere(center=[12.0*r,0,0],radius=r,material=mat5,fixed=True))
id10= O.bodies.append(sphere(center=[12.0*r,0,2*r],radius=r,material=mat5,fixed=True))

id11= O.bodies.append(sphere(center=[15.0*r,0,0],radius=r,material=mat6,fixed=True))
id12= O.bodies.append(sphere(center=[15.0*r,0,2*r],radius=r,material=mat6,fixed=True))


o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_ViscElCapMat_ViscElCapMat_ViscElCapPhys()],
    [Law2_ScGeom_ViscElCapPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,0]),
  PyRunner(command='addPlotData()',iterPeriod=100),
]

vel = 0.01
O.bodies[id2].state.vel=[0,0,vel]
O.bodies[id4].state.vel=[0,0,vel]
O.bodies[id6].state.vel=[0,0,vel]
O.bodies[id8].state.vel=[0,0,vel]
O.bodies[id10].state.vel=[0,0,vel]
O.bodies[id12].state.vel=[0,0,vel]

def addPlotData(): 
  f1=O.forces.f(id2)
  f2=O.forces.f(id4)
  f3=O.forces.f(id6)
  f4=O.forces.f(id8)
  f5=O.forces.f(id10)
  f6=O.forces.f(id12)
  
  s1 = (O.bodies[id2].state.pos[2]-O.bodies[id1].state.pos[2])-2*r
  sc=s1

  plot.addData(Willett_numeric=-f1[2], Willett_analytic=-f2[2], Rabinovich=-f3[2], Lambert=-f4[2], Weigert=-f5[2], Soulie=-f6[2], sc=sc)
  
  

plot.plots={'sc':('Willett_numeric','Willett_analytic','Rabinovich','Lambert','Weigert','Soulie')}; plot.plot()

O.step()
from yade import qt
qt.View()

O.run(250000, True)
#plot.saveGnuplot('sim-data_'+CapillarType1)
