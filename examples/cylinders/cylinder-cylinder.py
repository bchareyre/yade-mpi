# encoding: utf-8
"An example showing how to create two cylinders which are interacting."

from yade.gridpfacet import *

#### Parameter ####
L=1.		# length of the cylinder element
r=0.1		# radius of the cylinder element
phi=30.	# friction angle
E=1e6		# Young's modulus

#### Engines ####
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_GridConnection_Aabb(),
	]),
	InteractionLoop([
		Ig2_GridNode_GridNode_GridNodeGeom6D(),
		Ig2_GridConnection_GridConnection_GridCoGridCoGeom(),
	],
	[
		Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),	# internal cylinder physics
		Ip2_FrictMat_FrictMat_FrictPhys()	# physics for external interactions, i.e., cylinder-cylinder interaction
	],
	[
		Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),	# contact law for "internal" cylinder forces
		Law2_GridCoGridCoGeom_FrictPhys_CundallStrack()	# contact law for cylinder-cylinder interaction
	]
	),
	NewtonIntegrator(gravity=(-0.,0,-10),damping=0.5,label='newton'),
]

#### Creat materials ####
O.materials.append( CohFrictMat( young=E,poisson=0.3,density=1000,frictionAngle=radians(phi),normalCohesion=1e10,shearCohesion=1e10,momentRotationLaw=True,label='cMat' ) )  # material to create the gridConnections
O.materials.append( FrictMat( young=E,poisson=0.3,density=1000,frictionAngle=radians(phi),label='fMat' ) )  # material for general interactions

#### Create cylinders ####
nodesIds=[]
cylIds=[]
cylinder((0,0,0),(L,0,0),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=True,color=[1,0,0],intMaterial='cMat',extMaterial='fMat')
cylinder((L/4,2*L/3,L),(L/4,-L/3,L),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=False,color=[0,1,0],intMaterial='cMat',extMaterial='fMat')
cylinder((0,2*L/3,L),(0,-L/3,L),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=False,color=[0,1,0],intMaterial='cMat',extMaterial='fMat')
cylinder((L/2,L/2,L),(L/2,-L/2,L),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=False,color=[0,1,1],intMaterial='cMat',extMaterial='fMat')
cylinder((3*L/4,L/3,L),(3*L/4,-2*L/3,L),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=False,color=[0,0,1],intMaterial='cMat',extMaterial='fMat')
cylinder((L,L/3,L),(L,-2*L/3,L),radius=r,nodesIds=nodesIds,cylIds=cylIds,
				 fixed=False,color=[0,0,1],intMaterial='cMat',extMaterial='fMat')

#### For viewing ####
from yade import qt
qt.View()
Gl1_Sphere.stripes=True

#### Set a time step ####
O.dt=1e-06

#### Allows to reload the simulation ####
O.saveTmp()
