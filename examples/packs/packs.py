#!/usr/bin/python
# -*- coding: utf-8 -*-
from yade import pack,ymport,export,log,geom

""" This script demonstrates how to use 2 components of creating packings:

1. packing generators pack.regularHexa, pack.regularOrtho etc. generate vertices and filter them
using predicates. (Note that this will be enhanced to irregular packings in the future)

2. predicates are functors returning True/False for points that are given by the packing generator.
Their names are mostly self-explanatory, see their docstrings for meaning of their arguments.

Predicates can be combined using set arithmetics to get their Intersection (p1 & p2), union (p1 | p2),
difference (p1 - p2) and symmetric difference (XOR, p1 ^ p2). This is demontrated on the head (which
has sphere taken off at the back and also a notch) and the body (with cylidrical hole inside).

"""

rad,gap=.15,.02

#Add material
O.materials.append(FrictMat(young=10e9,poisson=.25,frictionAngle=0.5,density=1e3))

#Parameters, which will be passed into spheres and facets creators
kw={'material':0}
kwBoxes={'color':[1,0,0],'wire':False,'dynamic':False,'material':0}
kwMeshes={'color':[1,1,0],'wire':True,'dynamic':False,'material':0}

O.bodies.append(
	pack.regularHexa(
		(pack.inSphere((0,0,4),2)-pack.inSphere((0,-2,5),2)) & pack.notInNotch(centerPoint=(0,0,4),edge=(0,1,0),normal=(-1,1,-1),aperture=.2)
		,radius=rad,gap=gap,color=(0,1,0),material=0) # head
	+[utils.sphere((.8,1.9,5),radius=.2,color=(.6,.6,.6),material=0),utils.sphere((-.8,1.9,5),radius=.2,color=(.6,.6,.6),material=0),utils.sphere((0,2.4,4),radius=.4,color=(1,0,0),material=0)] # eyes and nose
	+pack.regularHexa(pack.inCylinder((-1,2.2,3.3),(1,2.2,3.3),2*rad),radius=rad,gap=gap/3,color=(0.929,0.412,0.412),material=0) #mouth
)
groundId=O.bodies.append(utils.facet([(12,0,-6),(0,12,-6,),(-12,-12,-6)],dynamic=False)) # ground

for part in [
	pack.regularHexa (
		pack.inAlignedBox((-2,-2,-2),(2,2,2))-pack.inCylinder((0,-2,0),(0,2,0),1),
		radius=1.5*rad,gap=2*gap,color=(1,0,1),**kw), # body,
	pack.regularOrtho(pack.inEllipsoid((-1,0,-4),(1,1,2)),radius=rad,gap=0,color=(0,1,1),**kw), # left leg
	pack.regularHexa (pack.inCylinder((+1,1,-2.5),(0,3,-5),1),radius=rad,gap=gap,color=(0,1,1),**kw), # right leg
	pack.regularHexa (pack.inHyperboloid((+2,0,1),(+6,0,0),1,.5),radius=rad,gap=gap,color=(0,0,1),**kw), # right hand
	pack.regularOrtho(pack.inCylinder((-2,0,2),(-5,0,4),1),radius=rad,gap=gap,color=(0,0,1),**kw) # left hand
	]: O.bodies.appendClumped(part)


# Example of geom.facetBox usage 
oriBody = Quaternion(Vector3(0,0,1),(3.14159/3))
O.bodies.append(geom.facetBox((12,0,-6+0.9),(1,0.7,0.9),oriBody,**kwBoxes))

oriBody = Quaternion(Vector3(0,0,1),(3.14159/2))
O.bodies.append(geom.facetBox((0,12,-6+0.9),(1,0.7,0.9),oriBody,**kwBoxes))

oriBody = Quaternion(Vector3(0,0,1),(3.14159))
O.bodies.append(geom.facetBox((-12,-12,-6+0.9),(1,0.7,0.9),oriBody,**kwBoxes))

# Example of geom.facetCylinder usage, RotationEngine example see below
oriBody = Quaternion(Vector3(0,0,1),(3.14159/2))
rotateIDs=O.bodies.append(geom.facetCylinder((6.0,6.0,-4.0),2.0,4.0,oriBody,wallMask=4,segmentsNumber=10,**kwBoxes))

oriBody = Quaternion(Vector3(0,0,1),(3.14159/2))
O.bodies.append(ymport.gmsh('cone.mesh',orientation=oriBody,**kwMeshes))#generates facets from the mesh file

oriBody = Quaternion(Vector3(0,0,1),(3.14159/2))
O.bodies.append(ymport.gengeoFile('LSMGenGeo.geo',shift=Vector3(-7.0,-7.0,0.0),scale=1.0,orientation=oriBody,color=(1,0,1),**kw))

#facetBunker Demonstration
#Demonstration of HarmonicMotionEngine
vibrationPlate = O.bodies.append(geom.facetBunker((-7.0,-7.0,-3.0),dBunker=5.0,dOutput=3.0,hBunker=11.0,hOutput=1.5,hPipe=0.8,wallMask=5,segmentsNumber=20,**kwMeshes))

# spheresToFile saves coordinates and radii of all spheres of the simulation into the text file, works but disabled. Please, uncomment it, if you need
#print "Saved into the OutFile " + str (export.text("OutFile")) + " spheres";

# spheresFromFile function imports coordinates and radii of all spheres of the simulation into the text file
O.bodies.append(ymport.text('foo.spheres',shift=Vector3(6.0,6.0,-2.9),scale=0.7,color=(1,1,1),**kw))

#Demonstration of HarmonicRotationEngine
O.bodies.append(pack.regularHexa(pack.inSphere((-15,5,-5),1.5),radius=rad*2.0,gap=rad/3.0,color=(0.5,0.5,0.1),material=0))
O.bodies.append(geom.facetBox((-15,5,-5),(2,2,2),wallMask=15,**kwMeshes))
vibrationRotationPlate = O.bodies.append(geom.facetBox((-15,5,-5),(2,2,2),wallMask=16,**kwBoxes))

O.bodies.append(utils.wall((0,0,-10),axis=2))

try:
	from yade import qt
	qt.Controller()
	qt.View()
except ImportError: pass

#log.setLevel('SubdomainBalancer',log.TRACE)

O.engines=[
	#SubdomainBalancer(colorize=True,initRun=True,iterPeriod=100),
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),Bo1_Wall_Aabb()],label='collider'),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom(),Ig2_Wall_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	GravityEngine(gravity=(1e-2,1e-2,-1000)),
	NewtonIntegrator(damping=.1,exactAsphericalRot=True),
	RotationEngine(
		ids=rotateIDs,
		angularVelocity=10.0,
		rotationAxis=[0,0,1],
		rotateAroundZero=1,
		zeroPoint=[6.0,6.0,0.0]),
	HarmonicMotionEngine(A=[0,0,0.5], f=[0,0,20.0], fi = [0.0,0.0,pi], ids = vibrationPlate),
	HarmonicRotationEngine(A=0.2, f=20.0, fi = pi, rotationAxis=[1.0,0.0,0.0], rotateAroundZero = True, zeroPoint = [-15.0,3.0,-7.0], ids = vibrationRotationPlate) 
]
# we don't care about physical accuracy here, (over)critical step is fine as long as the simulation doesn't explode
O.dt=utils.PWaveTimeStep()
O.saveTmp()
O.timingEnabled=True
#O.run(10000,True)
#from yade import timing
#timing.stats()
#quit()
