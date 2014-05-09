#!/usr/bin/env python
# encoding: utf-8

from yade import utils, plot, qt, ymport
o = Omega()

# Physical parameters
fr = 0.0;
rho=1000.0

k = 5.0
mu = 0.001
tc = 0.0001; en = 0.7; et = 0.7;
vel = 0.05
Rad = 15.0e-3
o.dt = 0.0002


scaleF =  0.001

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et))
id1 = O.bodies.append(ymport.gmsh("box.mesh", scale=scaleF, material=mat1, color=(1,0,0), mask = 1, wire=True))

d = 15.0*scaleF

print d
idSpheres = O.bodies.append(
  pack.regularHexa(
    pack.inAlignedBox(
    (0,       -2000.0*scaleF,  0.0),
    (1000*scaleF, 200*scaleF,  50.0*scaleF)),
    radius=d,gap=0.01*d, material=mat1, mask=1, color=(0,1,1)))


print len(idSpheres)

# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(label='is2aabb'),Bo1_Facet_Aabb(label='is3aabb')]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(label='ss2sc'),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0.05,gravity=[0,-9.81,0]),
  SPHEngine(mask=1, k=k, rho0 = rho),
  VTKRecorder(iterPeriod=100,fileName='./cpt/spheres-', recorders=['spheres','velocity','colors','intr','ids','mask','materialId','stress']),
  VTKRecorder(iterPeriod=100,fileName='./cpt/facet-',   recorders=['facets'],label='VTK_box2'),
  PyRunner(command='addPlotData()',iterPeriod=50,dead=False),
]

def addPlotData(): 
  plot.addData(t=O.time, Ekin=utils.kineticEnergy())

plot.plots={'t':('Ekin')}; plot.plot()

O.run(1, True)

qt.View()
#O.run(10000, True)
#plot.saveGnuplot('sim-data_0.05')
