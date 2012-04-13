# -*- coding: utf-8

from yade import utils,pack,export,qt,geom
import gts,os,random,itertools
from numpy import *

# Parameters
tc=0.001# collision time 
en=.3  # normal restitution coefficient
es=.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
# facets material
params=utils.getViscoelasticFromSpheresInteraction(tc,en,es)
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs
# default spheres material
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,**params)) 

O.dt=.05*tc # time step

Rs=0.1 # particle radius

# Create geometry
box = O.bodies.append(geom.facetBox((0,0,0),(1,1,1),wallMask=31,material=facetMat))

# Create clumps...
for j in xrange(10):
	clpId,sphId=O.bodies.appendClumped([utils.sphere(Vector3(0,Rs*2*i,(j+1)*Rs*2),Rs,material=dfltSpheresMat) for i in xrange(4)])
	

# ... and spheres
sphAloneId=O.bodies.append( [utils.sphere( Vector3(0.5,Rs*2*i,(j+1)*Rs*2), Rs, material=dfltSpheresMat) for i in xrange(4) ] )

# Create engines
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
]

renderer = qt.Renderer()
qt.View()
O.saveTmp()

