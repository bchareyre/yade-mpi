# encoding: utf-8
"""
Same example as CohesiveCylinderSphere.py but using gridConnections instead of chainedCylinder.
"""
from yade import qt
from yade.gridpfacet import *
from numpy import linspace


### Engines need to be defined first since the function gridConnection creates the interaction
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_GridConnection_Aabb(),
	]),
	InteractionLoop(
		# Geometric interactions
		[
          Ig2_GridNode_GridNode_GridNodeGeom6D(),
          Ig2_Sphere_GridConnection_ScGridCoGeom(),	# used for the cohesive sphere-cylinder interaction
        ],
		[
		# Interaction phusics
          Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),
        ],
		# Interaction law
		[
          Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
          Law2_ScGridCoGeom_CohFrictPhys_CundallStrack(),	# used for the cohesive sphere-cylinder interaction
        ]
	),
	NewtonIntegrator(gravity=(0,0,0),damping=0.3,label='newton'),
	PyRunner(command='main()',iterPeriod=1000),
]

O.dt=5e-07

O.materials.append(CohFrictMat(young=8e5,poisson=0.3,density=4e3,frictionAngle=radians(30),normalCohesion=1e5,shearCohesion=1e5,momentRotationLaw=True,label='gridNodeMat'))

O.materials.append(CohFrictMat(young=8e5,poisson=0.3,density=4e3,frictionAngle=radians(30),normalCohesion=1e5,shearCohesion=1e5,momentRotationLaw=True,label='gridCoMat'))

O.materials.append(CohFrictMat(young=8e5,poisson=0.3,density=4e3,frictionAngle=radians(30),normalCohesion=4e4,shearCohesion=1e5,momentRotationLaw=False,label='spheremat'))

rCyl=0.006
nL=15
L=0.3

### Create all nodes first :
nodesIds=[]
for i in linspace(0,L,nL):
  nodesIds.append( O.bodies.append(
    gridNode([i,0,0],rCyl,wire=False,fixed=False,material='gridNodeMat') ) )

### Now create connection between the nodes
for i,j in zip( nodesIds[:-1], nodesIds[1:]):
  O.bodies.append( gridConnection(i,j,rCyl,
                                  material='gridCoMat'
                                  ) )

### Set fixed nodes
O.bodies[nodesIds[0]].state.blockedDOFs='xyzXYZ'
O.bodies[nodesIds[-1]].state.blockedDOFs='xyzXYZ'

IdSphere=O.bodies.append(sphere([0.15,0,2.*rCyl],rCyl,wire=False,fixed=False,material='spheremat'))

def main():
	global Fn,Ft
	if O.iter>50000:
		O.bodies[IdSphere].dynamic=False
		O.bodies[IdSphere].state.vel[2]=0.1


qt.View()
O.saveTmp()


