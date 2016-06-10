#!/usr/bin/python
# -*- coding: utf-8 -*-
"Beam-like behaviour with cylinderConnections for roots interaction with spheres."

from yade import pack
from yade.gridpfacet import *

#### Parameter ####
young=4.0e6
poisson=3
density=1e3
frictionAngle1=radians(15)
frictionAngle2=radians(15)
frictionAngle3=radians(15)

#### Engines ####
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Box_Aabb(),
		Bo1_Sphere_Aabb(),
		Bo1_GridConnection_Aabb(),
	]),
	InteractionLoop([
		Ig2_Sphere_Sphere_ScGeom(),
		Ig2_Box_Sphere_ScGeom(),
		Ig2_GridNode_GridNode_GridNodeGeom6D(),
		Ig2_Sphere_GridConnection_ScGridCoGeom(),
	],
	[
		Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),	# internal cylinder physics
		Ip2_FrictMat_FrictMat_FrictPhys()	# physics for external interactions, i.e., cylinder-cylinder, sphere-sphere, cylinder-sphere
	],
	[
		Law2_ScGeom_FrictPhys_CundallStrack(),	# contact law for sphere-sphere, cylinder-sphere
		Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),	# contact law for "internal" cylider forces
		Law2_GridCoGridCoGeom_FrictPhys_CundallStrack()	# contact law for cylinder-cylinder interaction
	]
	),
	GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.1,label='ts'), 
	NewtonIntegrator(gravity=(1,-9.81,0),damping=0.5,label='newton'),
]

#### Spheres ####
O.materials.append(FrictMat(young=4.0e6,poisson=0.5,frictionAngle=frictionAngle1,density=1600,label='spheremat'))

Ns=90
sp=pack.SpherePack()

if os.path.exists("cloud4cylinders"+`Ns`):
 	print "loading spheres from file"
	sp.load("cloud4cylinders"+`Ns`)
else:
 	print "generating spheres"
 	Ns=sp.makeCloud(Vector3(-0.3,0.2,-1.0),Vector3(+0.3,+0.5,+1.0),-1,.2,Ns,False,0.8)
	sp.save("cloud4cylinders"+`Ns`)

O.bodies.append([sphere(center,rad,material='spheremat') for center,rad in sp])

#### Walls ####
O.materials.append(FrictMat(young=1.0e6,poisson=0.2,density=2.60e3,frictionAngle=frictionAngle2,label='wallmat'))
walls=aabbWalls((Vector3(-0.3,-0.15,-1),Vector3(+0.3,+1.0,+1)),thickness=.1,material='wallmat')
wallIds=O.bodies.append(walls)

#Assemble cylinders in sinusoidal shapes
O.materials.append( CohFrictMat( young=10*young,poisson=poisson,density=density,frictionAngle=frictionAngle3,normalCohesion=1e40,shearCohesion=1e40,momentRotationLaw=True,label='cylindermat') )  # material to create the gridConnections
O.materials.append( FrictMat( young=young,poisson=poisson,density=13.0*density,frictionAngle=frictionAngle3,label='fMat' ) )  # material for general interactions with cylinders

Ne=30	# number of cylinders
dy=0.03
dx=0.2
dz=0.2
Nc=1	# number of additional cylinderConnection

nodesIds=[]
cylIds=[]
for j in range(-Nc, Nc+1):
	dyj = abs(float(j))*dy
	dxj = abs(float(j))*dx
	dzj = float(j)*dz
	vertices=[]
	for i in range(0, Ne+1):
		omega=20/float(Ne); hy=0.0; hz=0.05; hx=1.5;
		px=float(i)*hx/float(Ne)-0.8+dxj; py=sin(float(i)*omega)*hy+dyj; pz=cos(float(i)*omega)*hz+dzj;
		vertices.append([pz,py,px])
	cylinderConnection(vertices,0.02,nodesIds,cylIds,color=[1,0,0],highlight=False,intMaterial='cylindermat',extMaterial='fMat')
	O.bodies[nodesIds[-1]].state.blockedDOFs='xyzXYZ'
	O.bodies[nodesIds[-Ne-1]].state.blockedDOFs='xyzXYZ'

#### For viewing ####
from yade import qt
qt.View()
Gl1_Sphere.stripes=True

#### Allows to reload the simulation ####
O.saveTmp()

