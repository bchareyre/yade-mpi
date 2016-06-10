# encoding: utf-8
"An example showing various bending beams."

from yade.gridpfacet import *

#### Parameter ####
L=10.		# length of the beam
n=12		# number of nodes used to generate the beam
r=L/50.	# radius of the beam element

#### Engines ####
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_GridConnection_Aabb()]),
	InteractionLoop(
		[Ig2_GridNode_GridNode_GridNodeGeom6D()],
		[Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False)],
		[Law2_ScGeom6D_CohFrictPhys_CohesionMoment()]
	),
	NewtonIntegrator(gravity=(0,0,-10),damping=0.5,label='newton')
]

#### Create materials and set different properties ####
O.materials.append(CohFrictMat(young=1e6,poisson=0.3,density=1e1,frictionAngle=10,normalCohesion=1e7,shearCohesion=1e7,momentRotationLaw=False,label='mat1'))
O.materials.append(CohFrictMat(young=1e6,poisson=0.3,density=1e1,frictionAngle=10,normalCohesion=1e7,shearCohesion=1e7,momentRotationLaw=True,label='mat2'))
O.materials.append(CohFrictMat(young=3e6,poisson=0.3,density=1e1,frictionAngle=10,normalCohesion=1e7,shearCohesion=1e7,momentRotationLaw=True,label='mat3'))
O.materials.append(CohFrictMat(young=1e7,poisson=0.3,density=1e1,frictionAngle=10,normalCohesion=1e7,shearCohesion=1e7,momentRotationLaw=True,label='mat4'))

#### Vertices ####
vertices1=[]
vertices2=[]
vertices3=[]
vertices4=[]
for i in range(0,n):
  vertices1.append( [i*L/n,0,0] )
  vertices2.append( [i*L/n,1,0] )
  vertices3.append( [i*L/n,2,0] )
  vertices4.append( [i*L/n,3,0] )

#### Create cylinder connections ####
nodesIds=[]
cylIds=[]
cylinderConnection(vertices1,r,nodesIds,cylIds,color=[1,0,0],highlight=False,intMaterial='mat1')
cylinderConnection(vertices2,r,nodesIds,cylIds,color=[0,1,0],highlight=False,intMaterial='mat2')
cylinderConnection(vertices3,r,nodesIds,cylIds,color=[0,0,1],highlight=False,intMaterial='mat3')
cylinderConnection(vertices4,r,nodesIds,cylIds,color=[1,1,1],highlight=False,intMaterial='mat4')

#### Set boundary conditions ####
for i in range(0,4):
   O.bodies[nodesIds[i*n]].dynamic=False
#   O.bodies[nodesIds[i*n]].state.blockedDOFs='xyzXYZ'
#   O.bodies[nodesIds[i*n]].state.blockedDOFs='xyz'

#### For viewing ####
from yade import qt
qt.View()

#### Set a time step ####
O.dt=1e-05

#### Allows to reload the simulation ####
O.saveTmp()
