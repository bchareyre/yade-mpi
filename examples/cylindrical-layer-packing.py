#!/usr/bin/python
# -*- coding: utf-8 -*-
from math import *


# "instantiate" Omega, i.e. create proxy object to Omega and rootBody
o=Omega()

o.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	IGeomDispatcher([Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()]),
	IPhysDispatcher([Ip2_FrictMat_FrictMat_FrictPhys()]),
	ElasticContactLaw(),
	GlobalStiffnessTimeStepper(defaultDt=1e-4,active=True,timeStepUpdateInterval=500),
	AxialGravityEngine(axisPoint=(0,0,0),axisDirection=(1,0,0),acceleration=1e4),
	NewtonIntegrator(damping=.4)
]

rCenter=4
rBall=.2
wallThickness=2*rBall
wallSize=5*rCenter
wallDist=5*rBall
#central cylinder composed of several spheres
nMax=10
for n in range(nMax):
	x=wallDist*(n/nMax-.5)
	O.bodies.append(utils.sphere([x,0,0],rCenter))

#lateral walls, they have wallDist gap inbetween
wLat1=utils.box([0+.5*wallDist+.5*wallThickness,0,0],[.5*wallThickness,wallSize,wallSize]); o.bodies.append(wLat1);
wLat2=utils.box([0-.5*wallDist-.5*wallThickness,0,0],[.5*wallThickness,wallSize,wallSize]); o.bodies.append(wLat2);

#angle walls, they cross at the x-axis
wAng1=utils.box([0,0,0],[.55*wallDist,.5*wallThickness,wallSize*sqrt(2)]); wAng1.state.ori=Quaternion((1,0,0),pi/4); o.bodies.append(wAng1); 
wAng2=utils.box([0,0,0],[.55*wallDist,.5*wallThickness,wallSize*sqrt(2)]); wAng2.state.ori=Quaternion((1,0,0),-pi/4); o.bodies.append(wAng2)

#cap
wCap=utils.box([0,0,wallSize],[.55*wallDist,wallSize,.5*wallThickness]); o.bodies.append(wCap)

# all bodies up to now are fixed and only wire is will be shown
for b in o.bodies:
	b.shape.wire=True
	b.dynamic=False

import random
def randomColor():
	return [random.random(),random.random(),random.random()]

### now the falling balls in 
maxima=[.5*(wallDist/rBall),rCenter/rBall,rCenter/rBall] # number of spheres in x,y,z directions
for ix in range(int(maxima[0])):
	for iy in range(int(maxima[1])):
		for iz in range(int(maxima[2])):
			x,y,z=2.0*rBall*(ix-maxima[0]/2+.5),2.0*rBall*(iy-maxima[1]/2+.5),2.0*rBall*iz+2*rCenter
			#print x,y,z,rBall
			O.bodies.append(utils.sphere([x,y,z],rBall))

o.save('/tmp/a.xml')

try:
	from yade import qt
	qt.Controller()
	qt.View()
except ImportError: pass

if 0:
	import os,time
	os.system(yadeExecutable+' -N QtGUI -S /tmp/a.xml')
else:
	o.run(30000);
	o.wait()
	o.save('/tmp/a_30000.xml')
	#os.system(yadeExecutable+' -N QtGUI -S /tmp/a_10000.xml')

