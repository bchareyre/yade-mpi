# encoding: utf-8
# Copyright (C) 2012 by Bruno Chareyre
# An update of the original script from Janek Kozicki

from yade import pack
from numpy import arange
import itertools
import random
import yade.plot

## corners of the initial packing
mn,mx=Vector3(0,0,0),Vector3(10,10,10)

## create material #0, which will be used as default
O.materials.append(FrictMat(young=6e6,poisson=.4,frictionAngle=radians(5),density=2600))
O.materials.append(FrictMat(young=6e6,poisson=.4,frictionAngle=0,density=2600,label='frictionless'))

d=8

# clumps
for xyz in itertools.product(arange(0,d),arange(0,d),arange(0,d)):
	ids_spheres=O.bodies.appendClumped(pack.regularHexa(pack.inEllipsoid((mn[0]+xyz[0]*(mx[0]-mn[0])/d,mn[0]+xyz[1]*(mx[1]-mn[1])/d,mn[2]+xyz[2]*(mx[2]-mn[2])/d),(0.45+random.random()*0.1,0.45+random.random()*0.1,0.45+random.random()*0.1)),radius=0.15+random.random()*0.05,gap=0,color=[random.random(),random.random(),random.random()]))

## create walls around the packing
walls=aabbWalls(material='frictionless')
wallIds=O.bodies.append(walls)

from yade import qt
qt.Controller()
qt.View()


## hope that we got the ids right?!
triax=TriaxialCompressionEngine(
	wall_bottom_id=wallIds[2],
	wall_top_id=wallIds[3],
	wall_left_id=wallIds[0],
	wall_right_id=wallIds[1],
	wall_back_id=wallIds[4],
	wall_front_id=wallIds[5],
	internalCompaction=False,
	sigmaIsoCompaction=50e3,
	sigmaLateralConfinement=50e3,
	strainRate=0.02,
	frictionAngleDegree=30,
	StabilityCriterion = 0.01,
	max_vel=1,
)

recorder=TriaxialStateRecorder(
	iterPeriod=20,
	file="./WallStresses_clumps",
	truncate=1
)
	

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.6),
	triax,
	recorder,
	# you can add TriaxialStateRecorder and such here…
	NewtonIntegrator(damping=.4)
]

#yade.plot.plots={'eps':('sigma',)}

#O.saveTmp('initial');
#def addPlotData():
#	yade.plot.addData({'t':O.time,'i':O.iter,'eps':strainer.strain,'sigma':strainer.avgStress})

