# -*- coding: utf-8
from yade import qt,plot
from yade.gridpfacet import *

##################
### PARAMETERS ###
##################
phi=20.
E=3.*1e8
color=[255./255.,102./255.,0./255.]
r=0.005

# position of imported mesh
xpafet=0.22
ypafet=0.05


################
### ENGINES  ###
################
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_PFacet_Aabb(), 
	]),	
	InteractionLoop([
		Ig2_GridNode_GridNode_GridNodeGeom6D(),
		Ig2_GridConnection_GridConnection_GridCoGridCoGeom(),
		Ig2_Sphere_PFacet_ScGridCoGeom(),
		Ig2_PFacet_PFacet_ScGeom(),
    ],	
    [
        Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True),
		Ip2_FrictMat_FrictMat_FrictPhys()
	],
	[
        Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
		Law2_ScGeom_FrictPhys_CundallStrack(),
		Law2_ScGridCoGeom_FrictPhys_CundallStrack(),
		Law2_GridCoGridCoGeom_FrictPhys_CundallStrack()
	]
	),
    GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.5,label='ts'), 
	NewtonIntegrator(gravity=(0,-9.81,0),damping=0.0,label='newton'),
	PyRunner(iterPeriod=200,command='history()'),
]


################
### MATERIAL ###
################
O.materials.append( CohFrictMat( young=E,poisson=0.3,density=2650,frictionAngle=radians(phi),normalCohesion=3e100,shearCohesion=3e100,momentRotationLaw=True,label='gridNodeMat' ) )  # material to create the gridConnections
O.materials.append( FrictMat( young=E,poisson=0.3,density=2650,frictionAngle=radians(phi),label='pFacetMat' ) )  # material for general interactions


########################################
### GENERATE THE WALL AND THE SPHERE ###
########################################

# FIXED WALL
O.bodies.append( gridNode([-0.25,-0.22,-.25],r,wire=False,fixed=True,material='gridNodeMat',color=color) )
O.bodies.append( gridNode([.5,0.,-.25],r,wire=False,fixed=True,material='gridNodeMat',color=color) )
O.bodies.append( gridNode([-.25,-0.22,.25],r,wire=False,fixed=True,material='gridNodeMat',color=color) )
O.bodies.append( gridNode([.5,0.,.25],r,wire=False,fixed=True,material='gridNodeMat',color=color) )
O.bodies.append( gridConnection(0,1,r,color=color,material='gridNodeMat') )
O.bodies.append( gridConnection(2,3,r,color=color,material='gridNodeMat') )
O.bodies.append( gridConnection(2,1,r,color=color,material='gridNodeMat') )
O.bodies.append( gridConnection(2,0,r,color=color,material='gridNodeMat') )
O.bodies.append( gridConnection(3,1,r,color=color,material='gridNodeMat') )
O.bodies.append( pfacet(2,1,0,wire=False,material='pFacetMat',color=color) )
O.bodies.append( pfacet(2,3,1,wire=False,material='pFacetMat',color=color) )

# IMPORT MESH
oriBody = Quaternion(Vector3(0,0,1),pi/2.)
nodesIds,cylIds,pfIds = gmshPFacet( meshfile='sphere.mesh', shift=Vector3(xpafet,ypafet,0.), orientation=oriBody, radius=r, wire=False, fixed=False, materialNodes='gridNodeMat', material='pFacetMat', color=[1,0,0] )


############
### PLOT ###
############
def history():
  xyz=[]
  for k in [0,1,2]:
    ksum=0
    for i in nodesIds:
      ksum+=O.bodies[i].state.pos[k]
    xyz.append(ksum/len(nodesIds))  # take average value as reference
  plot.addData(i=O.iter,t=O.time,x=xyz[0],y=xyz[1],z=xyz[2])
plot.plots={'x':'y'}
plot.plot()


##########
## VIEW ##
##########

qt.Controller()
qtv = qt.View()
qtr = qt.Renderer()
qtr.light2=True
qtr.lightPos=Vector3(1200,1500,500)
qtr.bgColor=[1,1,1]

O.saveTmp()
