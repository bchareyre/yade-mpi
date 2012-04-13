# -*- coding: utf-8

from yade import utils,pack,export,qt
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
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle, **params)) 

O.dt=.05*tc # time step

Rs=0.05 # particle radius

# Create geometry

x0=0.; y0=0.; z0=0.; ab=.7; at=2.; h=1.; hl=h; al=at*3

zb=z0;   x0b=x0-ab/2.; y0b=y0-ab/2.; x1b=x0+ab/2.; y1b=y0+ab/2.
zt=z0+h; x0t=x0-at/2.; y0t=y0-at/2.; x1t=x0+at/2.; y1t=y0+at/2.
zl=z0-hl;x0l=x0-al/2.; y0l=y0-al/2.; x1l=x0+al/2.; y1l=y0+al/2.

left = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y0b,zb),Vector3(x0t,y0t,zt),Vector3(x0t,y1t,zt),Vector3(x0b,y1b,zb)]],capStart=True,capEnd=True)
lftIds=O.bodies.append(pack.gtsSurface2Facets(left,material=facetMat,color=(0,1,0)))

right = pack.sweptPolylines2gtsSurface([[Vector3(x1b,y0b,zb),Vector3(x1t,y0t,zt),Vector3(x1t,y1t,zt),Vector3(x1b,y1b,zb)]],capStart=True,capEnd=True)
rgtIds=O.bodies.append(pack.gtsSurface2Facets(right,material=facetMat,color=(0,1,0)))

near = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y0b,zb),Vector3(x0t,y0t,zt),Vector3(x1t,y0t,zt),Vector3(x1b,y0b,zb)]],capStart=True,capEnd=True)
nearIds=O.bodies.append(pack.gtsSurface2Facets(near,material=facetMat,color=(0,1,0)))

far = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y1b,zb),Vector3(x0t,y1t,zt),Vector3(x1t,y1t,zt),Vector3(x1b,y1b,zb)]],capStart=True,capEnd=True)
farIds=O.bodies.append(pack.gtsSurface2Facets(far,material=facetMat,color=(0,1,0)))

table = pack.sweptPolylines2gtsSurface([[Vector3(x0l,y0l,zl),Vector3(x0l,y1l,zl),Vector3(x1l,y1l,zl),Vector3(x1l,y0l,zl)]],capStart=True,capEnd=True)
tblIds=O.bodies.append(pack.gtsSurface2Facets(table,material=facetMat,color=(0,1,0)))

# Create clumps...
clumpColor=(0.0, 0.5, 0.5)
for k,l in itertools.product(arange(0,10),arange(0,10)):
	clpId,sphId=O.bodies.appendClumped([utils.sphere(Vector3(x0t+Rs*(k*4+2),y0t+Rs*(l*4+2),i*Rs*2+zt),Rs,color=clumpColor,material=dfltSpheresMat) for i in xrange(4)])

# ... and spheres
spheresColor=(0.4, 0.4, 0.4)
for k,l in itertools.product(arange(0,9),arange(0,9)):
	sphAloneId=O.bodies.append( [utils.sphere( Vector3(x0t+Rs*(k*4+4),y0t+Rs*(l*4+4),i*Rs*2.3+zt),Rs,color=spheresColor,material=dfltSpheresMat) for i in xrange(4) ] )

# Create engines
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0, gravity=[0,0,-9.81]),
	#VTKRecorder(virtPeriod=0.01,fileName='/tmp/',recorders=['spheres','velocity','facets'])
]

from yade import qt
qt.View()
O.saveTmp()

