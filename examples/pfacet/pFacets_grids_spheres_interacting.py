# -*- coding: utf-8
# author: marcus Guadagnin Moravia <marcus.moravia@3sr-grenoble.fr>
from yade import qt,plot
from yade.gridpfacet import *

phi=20.
E=3.*1e8
r=0.005

O.engines=[
  ForceResetter(),
  InsertionSortCollider([
    Bo1_PFacet_Aabb(),
    Bo1_Sphere_Aabb(),
    Bo1_GridConnection_Aabb()
  ]),
  InteractionLoop([
    Ig2_GridNode_GridNode_GridNodeGeom6D(),
    Ig2_Sphere_PFacet_ScGridCoGeom(),
    Ig2_GridConnection_GridConnection_GridCoGridCoGeom(),
    Ig2_Sphere_GridConnection_ScGridCoGeom(),
    Ig2_GridConnection_PFacet_ScGeom(),
    Ig2_PFacet_PFacet_ScGeom(),
    Ig2_Sphere_Sphere_ScGeom(),
  ],
  [
    Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True),
    Ip2_FrictMat_FrictMat_FrictPhys()
  ],
  [
    Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
    Law2_ScGeom_FrictPhys_CundallStrack(),
    Law2_ScGridCoGeom_FrictPhys_CundallStrack(),
  ]
  ),
  GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.1,label='ts'),
  NewtonIntegrator(gravity=(0,-9.81,0),damping=.5,label='newton'),
]

O.materials.append( CohFrictMat( young=0.05*E,poisson=0.3,density=2650,frictionAngle=radians(phi),normalCohesion=3e100,shearCohesion=3e100,momentRotationLaw=True,label='gridNodeMat' ) )
O.materials.append( FrictMat( young=E,poisson=0.5,density=260050,frictionAngle=radians(phi),label='sphereMat' ) )

# SPHERES
O.bodies.append( sphere(center=(Vector3(0.025,-0.035,0.015)),radius=.005,material='sphereMat',fixed=False) )
O.bodies.append( sphere(center=(Vector3(0.045,-0.035,0.015)),radius=.005,material='sphereMat',fixed=False) )
O.bodies.append( sphere(center=(Vector3(0.045,-0.025,0.015)),radius=.005,material='sphereMat',fixed=False) )

# GRID
L=0.1 #length
l=0.05 #width
nbL=10 #number of nodes for the length
nbl=5 #number of nodes for the width
r=L/100. #radius
color=[40./255.,102./255.,50./255.]
nodesIds=[]
#Creating all nodes:
for i in range(0,nbL):
  for j in range(0,nbl):
    nodesIds.append( O.bodies.append(gridNode([i*L/nbL,-0.1,j*l/nbl],r,wire=False,fixed=False,material='gridNodeMat',color=color)) )
#Creating connection between the nodes
for i in range(0,len(nodesIds)):
  for j in range(i+1,len(nodesIds)):
    dist=(O.bodies[nodesIds[i]].state.pos - O.bodies[nodesIds[j]].state.pos).norm()
    if(dist<=L/nbL*1.01):
      O.bodies.append( gridConnection(nodesIds[i],nodesIds[j],r,material='gridNodeMat',color=color) )
#PFacet Connection
O.bodies.append( gridConnection(nodesIds[11],nodesIds[17],r,color=color,material='gridNodeMat') )
#PFacet
O.bodies.append( pfacet(nodesIds[11],nodesIds[17],nodesIds[16],wire=False,material='sphereMat',color=color) )
O.bodies.append( pfacet(nodesIds[11],nodesIds[12],nodesIds[17],wire=False,material='sphereMat',color=color) )
# Constraining the ends of the geogrid
for i in range(nodesIds[0],nodesIds[4]):
  O.bodies[i].state.blockedDOFs='xyzXZ'
  for j in range(nodesIds[-1]-(5),nodesIds[-1]+1):
    O.bodies[j].state.blockedDOFs='xyzXZ'

qt.Controller()
qtv = qt.View()
qtr = qt.Renderer()
qtr.bgColor=[1,1,1]
Gl1_Sphere.stripes=1
O.saveTmp()
