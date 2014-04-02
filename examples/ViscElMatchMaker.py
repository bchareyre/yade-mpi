#!/usr/bin/env python
# encoding: utf-8

# This example shows, how matchmaker can be used to
# set the parameters of ViscoElastic model.

from yade import utils, plot
o = Omega()
fr = 0.5;rho=2000
tc = 0.001; en = 0.5; et = 0.5; o.dt = 0.002*tc


r1 = 0.002381
r2 = 0.002381
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,tc=tc,en=en,et=et,density=rho))
mat2 = O.materials.append(ViscElMat(frictionAngle=fr,tc=tc,en=en,et=et,density=rho))
mat3 = O.materials.append(ViscElMat(frictionAngle=fr,tc=tc,en=en,et=et,density=rho))


id11 = O.bodies.append(sphere(center=[0,0,0],radius=r1,material=mat1,fixed=True,color=[0,0,1]))
id12 = O.bodies.append(sphere(center=[0,0,(r1+r2+0.005*r2)],radius=r2,material=mat2,fixed=False,color=[0,0,1]))

id21 = O.bodies.append(sphere(center=[3*r1,0,0],radius=r1,material=mat1,fixed=True,color=[0,1,0]))
id22 = O.bodies.append(sphere(center=[3*r1,0,(r1+r2+0.005*r2)],radius=r2,material=mat3,fixed=False,color=[0,1,0]))

id31 = O.bodies.append(sphere(center=[6*r1,0,0],radius=r1,material=mat2,fixed=True,color=[1,0,0]))
id32 = O.bodies.append(sphere(center=[6*r1,0,(r1+r2+0.005*r2)],radius=r2,material=mat3,fixed=False,color=[1,0,0]))

o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=(r1+r2)*5.0),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys( 
      en=MatchMaker(matches=((mat1,mat2,.9),(mat1,mat3,.5),(mat2,mat3,.1))),          # Set parameters
      et=MatchMaker(matches=((mat1,mat2,.9),(mat1,mat3,.5),(mat2,mat3,.1)))
      )],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
  PyRunner(command='addPlotData()',iterPeriod=100),
]

vel=-0.50
O.bodies[id12].state.vel=[0,0,vel]
O.bodies[id22].state.vel=[0,0,vel]
O.bodies[id32].state.vel=[0,0,vel]

def addPlotData():
  s1 = (O.bodies[id12].state.pos[2]-O.bodies[id11].state.pos[2])-(r1+r2)
  s2 = (O.bodies[id22].state.pos[2]-O.bodies[id11].state.pos[2])-(r1+r2)
  s3 = (O.bodies[id32].state.pos[2]-O.bodies[id11].state.pos[2])-(r1+r2)
  plot.addData(mat1mat2=s1,mat1mat3=s2,mat2mat3=s3,it=O.iter)
  
  

plot.plots={'it':('mat1mat2','mat1mat3','mat2mat3')}; plot.plot()

O.step()
from yade import qt
qt.View()

#O.run(100000, True)
#plot.saveGnuplot('sim-data_')
