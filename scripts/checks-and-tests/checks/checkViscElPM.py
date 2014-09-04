#!/usr/bin/env python
# encoding: utf-8

################################################################################
# CONSTITUTIVE LAW TESTING: Law2_ScGeom_ViscElPhys_Basic()
#
# Two spheres with velocities (v,0,0) and (-v,0,0) collide.
# This script checks if:
#  - the numerical displacement equals the analytical displacement at a certain
#    time before the spheres separate, for instance in t = 0.05 s
#    This also implies the consistency of the results in terms of velocity
#    (and acceleration).
#  - the normal stiffness and normal damping coefficients have been calculated
#    correctly in function of the normal coefficient (en) of restitution and the
#    impact time (tc); this is a consequence of the previous condition.
#
# Notice that:
#  - this script only checks the displacement before the separation because the
#    analytical solution (given below) supposes that the damping term is still
#    present, when the spheres separate.  This is however not true in the
#    numerical model (see source code, prevent appearing of attraction forces
#    due to a viscous component).
#  - this script does not check the tangential (or shear) behaviour of
#    the constitutive law.
#

from yade import plot,ymport
import math


################################################################################
# MATERIAL

rho = 2000 # [kg/m^3] density
mu  = 0.75 # [-]      friction coefficient
tc  = 0.2  # [s]      contact time
en  = 0.3  # [-]      normal restitution coefficient
et  = 0.2  # [-]      tangential restitution coefficient

frictionAngle = math.atan(mu)
mat = O.materials.append(ViscElMat(tc=tc,en=en,et=et,
                                   frictionAngle=frictionAngle,density=rho))


################################################################################
# GEOMETRY

r   = 0.1  # [m] sphere radius

b1 = O.bodies.append(utils.sphere(center=(2*r,0,0),radius=r,material=mat))
b2 = O.bodies.append(utils.sphere(center=(0,0,0),radius=r,material=mat))


################################################################################
# SIMULATION

O.dt = 1e-5 # [s]   fixed time step
v    = 2    # [m/s] velocity along x-axis

O.bodies[b1].state.vel[0] = -v
O.bodies[b2].state.vel[0] =  v

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

  NewtonIntegrator(damping=0)
]


################################################################################
# RUN

O.run(5001,True)


################################################################################
# COMPARE ANALYTICAL AND NUMERICAL SOLUTIONS

m = 4./3. * math.pi * r**3 * rho # [kg] mass of the sphere


# Normal stiffness and damping coefficients according to [Pournin2001]
meff = m/2
kn   = 2.0 * meff/tc**2 * (math.pi**2 + math.log(en)**2)
cn   = -4.0 * meff/tc * math.log(en)


# Analytical solution of a linear spring damper system
omega0      = math.sqrt(kn/m)
zeta        = cn / (2 * math.sqrt(kn * m))
omegad      = omega0 * math.sqrt(1 - zeta**2)
xAnalytical = v/omegad * math.exp(-zeta*omega0*O.time) * math.sin(omegad*O.time)


# Comparison (if ok, resultStatus is not incremented)
tolerance = 0.0001

xNumerical = O.bodies[b2].state.pos[0]

if ((abs(xNumerical-xAnalytical)/xAnalytical)>tolerance):
  resultStatus += 1

