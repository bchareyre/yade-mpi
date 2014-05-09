#!/usr/bin/env python
# encoding: utf-8

# This example allows to test different kernel functions
from yade import utils, plot, qt
o = Omega()

# Physical parameters
fr = 0.5;
rho=1000.0

k = 1.0
mu = 100.0
tc = 0.0001; en = 0.7; et = 0.7;
vel = 0.05
Rad = 15.0e-3
o.dt = 0.0001

shift = 1.0
# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 1))
mat2 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 2))
mat3 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 3))
mat4 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 4))
mat5 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 5))

# Add spheres
#inert = True
inert = True
id11 = O.bodies.append(sphere(center=[0,0,0],radius=Rad,                       material=mat1, mask = 1, fixed=True))
id12 = O.bodies.append(sphere(center=[0,0,(Rad*2.0*shift)],radius=Rad,          material=mat1, mask = 1, fixed=inert))

id21 = O.bodies.append(sphere(center=[3.0*Rad,0,0],radius=Rad,                 material=mat2, mask = 1, fixed=True))
id22 = O.bodies.append(sphere(center=[3.0*Rad,0,(Rad*2.0*shift)],radius=Rad,    material=mat2, mask = 1, fixed=inert))

id31 = O.bodies.append(sphere(center=[6.0*Rad,0,0],radius=Rad,                 material=mat3, mask = 1, fixed=True))
id32 = O.bodies.append(sphere(center=[6.0*Rad,0,(Rad*2.0*shift)],radius=Rad,    material=mat3, mask = 1, fixed=inert))

id41 = O.bodies.append(sphere(center=[9.0*Rad,0,0],radius=Rad,                 material=mat4, mask = 1, fixed=True))
id42 = O.bodies.append(sphere(center=[9.0*Rad,0,(Rad*2.0*shift)],radius=Rad,    material=mat4, mask = 1, fixed=inert))

id51 = O.bodies.append(sphere(center=[12.0*Rad,0,0],radius=Rad,                material=mat5, mask = 1, fixed=True))
id52 = O.bodies.append(sphere(center=[12.0*Rad,0,(Rad*2.0*shift)],radius=Rad,   material=mat5, mask = 1, fixed=inert))

vel = 0.1
O.bodies[id12].state.vel=Vector3(0,0,-vel)
O.bodies[id22].state.vel=Vector3(0,0,-vel)
O.bodies[id32].state.vel=Vector3(0,0,-vel)
O.bodies[id42].state.vel=Vector3(0,0,-vel)
O.bodies[id52].state.vel=Vector3(0,0,-vel)

# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(label='is2aabb')]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(label='ss2sc')],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
  SPHEngine(mask=1, k=k, rho0 = rho, KernFunctionDensity=1),
  PyRunner(command='addPlotData()',iterPeriod=1,dead=False),
]

print "Time\tX\tRho\tP\tFpr "

# Function to add data to plot
def addPlotData(): 
  #print "%.2f\t%.5f\t%.5f\t%.5f\t%.5f" % (O.time+O.dt, O.bodies[id2].state.pos[2], O.bodies[id2].rho, O.bodies[id2].press, O.forces.f(id2)[2])
  s1 = (O.bodies[id12].state.pos[2]-O.bodies[id11].state.pos[2])-(Rad*2.0)
  s2 = (O.bodies[id22].state.pos[2]-O.bodies[id21].state.pos[2])-(Rad*2.0)
  s3 = (O.bodies[id32].state.pos[2]-O.bodies[id31].state.pos[2])-(Rad*2.0)
  s4 = (O.bodies[id42].state.pos[2]-O.bodies[id41].state.pos[2])-(Rad*2.0)
  s5 = (O.bodies[id52].state.pos[2]-O.bodies[id51].state.pos[2])-(Rad*2.0)
  
  f1 = O.forces.f(id12)[2]
  f2 = O.forces.f(id22)[2]
  f3 = O.forces.f(id32)[2]
  f4 = O.forces.f(id42)[2]
  f5 = O.forces.f(id52)[2]
  
  plot.addData(sc=O.iter, s1 = s1, s2 = s2, s3 = s3, s4 = s4, s5 = s5,
               fc=O.iter, f1 = f1, f2 = f2, f3 = f3, f4 = f4, f5 = f5)

plot.plots={'sc':('s1', 's2', 's3', 's4', 's5'), 'fc':('f1', 'f2', 'f3', 'f4', 'f5')}; plot.plot()

O.run(1, True)

qt.View()
O.run(1500)
