# -*- coding: utf-8

from yade import utils,pack,export,qt
import gts,os,random,itertools
from numpy import *
import yade.log 

#yade.log.setLevel('NewtonsDampedLaw',yade.log.TRACE)

# Parameters
tc=0.001# collision time 
en=.3  # normal restitution coefficient
es=.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
# facets material
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
facetMat=O.materials.append(SimpleViscoelasticMat(frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs
# default spheres material
dfltSpheresMat=O.materials.append(SimpleViscoelasticMat(density=density,frictionAngle=frictionAngle)) 

O.dt=.1*tc # time step

Rs=0.05 # particle radius

# Create geometry

x0=0.; y0=0.; z0=0.; ab=.7; at=2.; h=1.; hl=h; al=at*3

zb=z0;   x0b=x0-ab/2.; y0b=y0-ab/2.; x1b=x0+ab/2.; y1b=y0+ab/2.
zt=z0+h; x0t=x0-at/2.; y0t=y0-at/2.; x1t=x0+at/2.; y1t=y0+at/2.
zl=z0-hl;x0l=x0-al/2.; y0l=y0-al/2.; x1l=x0+al/2.; y1l=y0+al/2.

left = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y0b,zb),Vector3(x0t,y0t,zt),Vector3(x0t,y1t,zt),Vector3(x0b,y1b,zb)]],capStart=True,capEnd=True)
lftIds=O.bodies.append(pack.gtsSurface2Facets(left.faces(),material=facetMat,color=(0,1,0)))

right = pack.sweptPolylines2gtsSurface([[Vector3(x1b,y0b,zb),Vector3(x1t,y0t,zt),Vector3(x1t,y1t,zt),Vector3(x1b,y1b,zb)]],capStart=True,capEnd=True)
rgtIds=O.bodies.append(pack.gtsSurface2Facets(right.faces(),material=facetMat,color=(0,1,0)))

near = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y0b,zb),Vector3(x0t,y0t,zt),Vector3(x1t,y0t,zt),Vector3(x1b,y0b,zb)]],capStart=True,capEnd=True)
nearIds=O.bodies.append(pack.gtsSurface2Facets(near.faces(),material=facetMat,color=(0,1,0)))

far = pack.sweptPolylines2gtsSurface([[Vector3(x0b,y1b,zb),Vector3(x0t,y1t,zt),Vector3(x1t,y1t,zt),Vector3(x1b,y1b,zb)]],capStart=True,capEnd=True)
farIds=O.bodies.append(pack.gtsSurface2Facets(far.faces(),material=facetMat,color=(0,1,0)))

table = pack.sweptPolylines2gtsSurface([[Vector3(x0l,y0l,zl),Vector3(x0l,y1l,zl),Vector3(x1l,y1l,zl),Vector3(x1l,y0l,zl)]],capStart=True,capEnd=True)
tblIds=O.bodies.append(pack.gtsSurface2Facets(table.faces(),material=facetMat,color=(0,1,0)))

# Create clumps...
clumpColor=(0.0, 0.5, 0.5)
for k,l in itertools.product(arange(0,10),arange(0,10)):
	clpId,sphId=O.bodies.appendClumped([utils.sphere(Vector3(x0t+Rs*(k*4+2),y0t+Rs*(l*4+2),i*Rs*2+zt),Rs,color=clumpColor,material=dfltSpheresMat) for i in xrange(4)])
	for id in sphId:
		s=O.bodies[id]
		p=utils.getViscoelasticFromSpheresInteraction(s.state['mass'],tc,en,es)
		s.mat['kn'],s.mat['cn'],s.mat['ks'],s.mat['cs']=p['kn'],p['cn'],p['ks'],p['cs']
	#O.bodies[clpId].state.blockedDOFs=['rx','ry','rz']
	#O.bodies[clpId].state.blockedDOFs=['x','y']

# ... and spheres
spheresColor=(0.4, 0.4, 0.4)
for k,l in itertools.product(arange(0,9),arange(0,9)):
	sphAloneId=O.bodies.append( [utils.sphere( Vector3(x0t+Rs*(k*4+4),y0t+Rs*(l*4+4),i*Rs*2.3+zt),Rs,color=spheresColor,material=dfltSpheresMat) for i in xrange(4) ] )
	for id in sphAloneId:
		s=O.bodies[id]
		p=utils.getViscoelasticFromSpheresInteraction(s.state['mass'],tc,en,es)
		s.mat['kn'],s.mat['cn'],s.mat['ks'],s.mat['cs']=p['kn'],p['cn'],p['ks'],p['cs']
		#s.state.blockedDOFs=['rx','ry','rz']
		#s.state.blockedDOFs=['x','y']

# Create engines
O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB()]),
	InsertionSortCollider(nBins=5,sweepLength=.1*Rs),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(), InteractingFacet2InteractingSphere4SpheresContactGeometry()],
		[Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys()],
		[Law2_Spheres_Viscoelastic_SimpleViscoelastic()],
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonsDampedLaw(damping=0,exactAsphericalRot=True),
	#VTKRecorder(virtPeriod=0.01,fileName='/tmp/',recorders=['spheres','velocity','facets'])
]

renderer = qt.Renderer()
qt.View()
O.saveTmp()

