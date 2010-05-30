# -*- coding: utf-8

from yade import utils,pack,export,qt
import gts,os,random,itertools
from numpy import *
import yade.log 

#yade.log.setLevel('NewtonIntegrator',yade.log.TRACE)

# Parameters
tc=0.001# collision time 
en=.3  # normal restitution coefficient
es=.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
# facets material
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs
# default spheres material
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle)) 

O.dt=.1*tc # time step

Rs=0.1 # particle radius

# Create geometry
bottom = pack.sweptPolylines2gtsSurface([[Vector3(-1,-1,-1),Vector3(1,-1,-1),Vector3(1, 1, -1),Vector3(-1, 1, -1)]],capStart=True,capEnd=True)
btmIds=O.bodies.append(pack.gtsSurface2Facets(bottom.faces(),material=facetMat,color=(0,1,0)))

#top = pack.sweptPolylines2gtsSurface([[Vector3(-1,-1,1),Vector3(1,-1,1),Vector3(1, 1, 1),Vector3(-1, 1, 1)]],capStart=True,capEnd=True)
#topIds=O.bodies.append(pack.gtsSurface2Facets(top.faces(),material=facetMat,color=(0,1,0)))

left = pack.sweptPolylines2gtsSurface([[Vector3(-1,-1,-1),Vector3(1,-1,-1),Vector3(1, -1, 1),Vector3(-1, -1, 1)]],capStart=True,capEnd=True)
lftIds=O.bodies.append(pack.gtsSurface2Facets(left.faces(),material=facetMat,color=(0,1,0)))

right = pack.sweptPolylines2gtsSurface([[Vector3(-1,1,-1),Vector3(1,1,-1),Vector3(1, 1, 1),Vector3(-1, 1, 1)]],capStart=True,capEnd=True)
rgtIds=O.bodies.append(pack.gtsSurface2Facets(right.faces(),material=facetMat,color=(0,1,0)))

near = pack.sweptPolylines2gtsSurface([[Vector3(1,-1,-1),Vector3(1,1,-1),Vector3(1, 1, 1),Vector3(1, -1, 1)]],capStart=True,capEnd=True)
nearIds=O.bodies.append(pack.gtsSurface2Facets(near.faces(),material=facetMat,color=(0,1,0)))

far = pack.sweptPolylines2gtsSurface([[Vector3(-1,-1,-1),Vector3(-1,1,-1),Vector3(-1, 1, 1),Vector3(-1, -1, 1)]],capStart=True,capEnd=True)
farIds=O.bodies.append(pack.gtsSurface2Facets(far.faces(),material=facetMat,color=(0,1,0)))

# Create clumps...
for j in xrange(10):
	clpId,sphId=O.bodies.appendClumped([utils.sphere(Vector3(0,Rs*2*i,(j+1)*Rs*2),Rs,material=dfltSpheresMat) for i in xrange(4)])
	for id in sphId:
		s=O.bodies[id]
		p=utils.getViscoelasticFromSpheresInteraction(s.state.mass,tc,en,es)
		s.mat.kn,s.mat.cn,s.mat.ks,s.mat.cs=p['kn'],p['cn'],p['ks'],p['cs']

# ... and spheres
sphAloneId=O.bodies.append( [utils.sphere( Vector3(0.5,Rs*2*i,(j+1)*Rs*2), Rs, material=dfltSpheresMat) for i in xrange(4) ] )
for id in sphAloneId:
	s=O.bodies[id]
	p=utils.getViscoelasticFromSpheresInteraction(s.state.mass,tc,en,es)
	s.mat.kn,s.mat.cn,s.mat.ks,s.mat.cs=p['kn'],p['cn'],p['ks'],p['cs']

# Create engines
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonIntegrator(damping=0,exactAsphericalRot=True),
]

renderer = qt.Renderer()
qt.View()
O.saveTmp()

