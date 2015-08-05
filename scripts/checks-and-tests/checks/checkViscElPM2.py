#!/usr/bin/env python
# encoding: utf-8

################################################################################
# CONSTITUTIVE LAW TESTING: Law2_ScGeom_ViscElPhys_Basic()
#
# Two spheres of different diameter and young modulus, with velocities 
# (v,0,0) and (-v,0,0) collide.
# The script checks if the restitution coefficient applied is the one obtained
# Law2_ScGeom_ViscElPhys_Basic() compute the stiffness and damping coefficient
# differently depending on the input. This test is then different from 
# checkViscElPM.py and checkViscElEng.py
#

from yade import plot,ymport
import math


################################################################################
# MATERIAL
diameter1 = 1e-2 #[m] diameter of the first particle
diameter2 = 1e-3    #[m] diameter of the second particle
rho1 = 1500 # [kg/m^3] density of the first particle
rho2 = 2500 # [kg/m^3] density of the second particle
youngMod = 1e5 # [kg/m/s^2] young modulus of the particles
poissonRatio = 0.5 # [-] poisson's ratio of the particles
mu  = 0.4 # [-]      friction coefficient of both particle
en  = 0.5  # [-]     normal restitution coefficient of both particle
et  = 1.  # [-]      tangential restitution coefficient of both particle, no damping

frictionAngle = math.atan(mu)

O.materials.append(ViscElMat(en=en,et=1.,young = youngMod,poisson = poissonRatio,frictionAngle=frictionAngle,density=rho1, label='Mat1'))
O.materials.append(ViscElMat(en=en,et=1.,young = youngMod,poisson = poissonRatio,frictionAngle=frictionAngle,density=rho2, label='Mat2'))

################################################################################
v = 1e-1
# GEOMETRY
O.bodies.append(sphere((0,0,0),diameter1/2.0, material = 'Mat1'))
O.bodies[0].state.vel[2] = v
O.bodies.append(sphere((0,0,(diameter1+diameter2)/2.+ 0.01*diameter1),diameter2/2.0, material = 'Mat2'))
O.bodies[-1].state.vel[2] = -v


################################################################################
# SIMULATION

O.dt = 1e-6 # [s]   fixed time step

O.engines=[
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),Bo1_Wall_Aabb()]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(),
     Ig2_Facet_Sphere_ScGeom(),
     Ig2_Wall_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()]
  ),
  NewtonIntegrator(damping=0.),
  PyRunner(command = 'check()',iterPeriod = int(0.05/O.dt)-1)
]

################################################################################
def check():
	global resultStatus
	# Compare imposed restitution coefficient and obtained one
	enMeasured = (O.bodies[1].state.vel[2]-O.bodies[0].state.vel[2])/(2*v)
	tolerance = 1e-2
	if (abs(enMeasured -en)/en)>tolerance:
		resultStatus+=1
################################################################################
# RUN
O.run(int(0.05/O.dt))
O.wait()
################################################################################


