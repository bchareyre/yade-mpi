#!/usr/bin/env python
# encoding: utf-8
#
# This script plots the viscous and normal forces of the contact
# of 2 spheres

from yade import utils, plot, qt
o = Omega()

# Physical parameters
fr = 0.5;rho=2000
tc = 0.0001; en = 0.7; et = 0.7;
o.dt = 0.000002*tc
Rad = 2.0e-3

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,tc=tc,en=en,et=et))

# Add spheres
id1 = O.bodies.append(sphere(center=[0,0,0],radius=Rad,material=mat1,fixed=True))
id2 = O.bodies.append(sphere(center=[0,0,(2.0*Rad)],radius=Rad,material=mat1,fixed=False))

O.bodies[id2].state.vel[2] = -1.0
# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
  PyRunner(command='addPlotData()',iterPeriod=100),
]

# Function to add data to plot
def addPlotData(): 
  try:
    delta = (O.bodies[id2].state.pos[2]-O.bodies[id1].state.pos[2])-(2*Rad)
    plot.addData(delta=delta, time1=O.time, time2=O.time, time3=O.time, time4=O.time,
              Fn = O.interactions[0,1].phys.Fn,
              Fv = O.interactions[0,1].phys.Fv,
              deltaDot = O.bodies[id2].state.vel[2] - O.bodies[id1].state.vel[2])
  except:
    pass
  
plot.plots={'time1':('delta'), 'time2':('deltaDot'), 'time3':('Fn'), 'time4':('Fv')}; plot.plot()

O.run(1)
qt.View()

#O.wait() ; plot.saveGnuplot('sim-data_Sphere')
