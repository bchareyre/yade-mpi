# encoding: utf-8

from yade import pack,geom,qt
from yade.gridpfacet import *
from pylab import *


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_GridConnection_Aabb()]),
	InteractionLoop(
		[Ig2_GridNode_GridNode_GridNodeGeom6D()],
		[Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False)],
		[Law2_ScGeom6D_CohFrictPhys_CohesionMoment()]
	),
	NewtonIntegrator(gravity=(0,0,-10),damping=0.1,label='newton')
]


O.materials.append(CohFrictMat(young=3e2,poisson=0.3,density=1e1,frictionAngle=10,normalCohesion=1e7,shearCohesion=1e7,momentRotationLaw=True,label='mat'))


### Parameters ###
L=0.1 #length [m]
n=10	#number of nodes for the length	[#]
r=L/100.	#radius
color=[255./255.,102./255.,0./255.]

### Create all nodes first
nodeIds=[]
for i in range(0,n):
  nodeIds.append( O.bodies.append( gridNode([i*L/n,0,0],r,wire=False,fixed=False,material='mat',color=color) ) )

### Create connections between the nodes
connectionIds=[]
for i,j in zip(nodeIds[:-1],nodeIds[1:]):
  connectionIds.append( O.bodies.append( gridConnection(i,j,r,color=color) ) )

### Set a fixed node
O.bodies[0].dynamic=False

O.dt=1e-06
O.saveTmp()
qt.View()


