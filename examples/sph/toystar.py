#!/usr/bin/env python
# encoding: utf-8

# This script demonstrates SPH-engine in Yade
# !!! Very experimental at the moment!!!

from yade import utils, plot, qt
o = Omega()

# Physical parameters
fr = 0.5;
rho=1000.0

k = 1.0
mu = 0.01
tc = 0.0001; en = 0.7; et = 0.7;
Rad = 10.0e-3
o.dt = 0.00001

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et))

# Add spheres
d = 0.8
idSpheres = O.bodies.append(
  pack.regularHexa(
    pack.inSphere(
    (0,0,0), d),
    radius=Rad,gap=10.5*Rad, material=mat1, mask=1, color=(0,1,1)))

idCentralBody = O.bodies.append(sphere(center=[0,0,0],radius=0.1*Rad, mask = 2, color = [1,0,0], fixed=True))


# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(label='is2aabb'),Bo1_Facet_Aabb(label='is3aabb')]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(label='ss2sc'),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  CentralGravityEngine(accel=50.0, label='gr', centralBody=idCentralBody),
  NewtonIntegrator(damping=0.1),
  SPHEngine(mask=1, k=k, rho0 = rho),
  VTKRecorder(iterPeriod=1000, mask=1, fileName='./cpt/spheres-', recorders=['spheres','velocity','colors','intr','ids','mask','materialId','stress']),
]


O.step()
qt.View()
