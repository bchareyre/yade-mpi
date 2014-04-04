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
mu = 0.001
tc = 0.0001; en = 0.7; et = 0.7;
Rad = 10.0e-3
o.dt = 0.00025

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True,mu=mu,tc=tc, en=en, et=et))

# Add spheres
d = 0.18
idSpheres = O.bodies.append(
  pack.regularHexa(
    pack.inAlignedBox(
    (-d,-d,-d), # lower angle
    (d,d,d)),   # upper angle
    radius=Rad,gap=0.2*Rad, material=mat1,color=(0,1,1)))
    

id1 = O.bodies.append(geom.facetBox((Vector3(0.0,0,0.0)),
  (Vector3(0.2, 0.2, 0.2)),
  material=mat1, mask=1, color=(1,0,0), wire=True))

# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(label='is2aabb'),Bo1_Facet_Aabb(label='is3aabb')]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(label='ss2sc'),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0.0,gravity=[0,0,-9.81]),
  SPHEngine(mask=1, k=k, rho0 = rho),
]


O.step()
qt.View()
