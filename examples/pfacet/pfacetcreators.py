# encoding: utf-8
from yade import qt
from yade.gridpfacet import *


###########################
#####   ENGINES       #####
###########################

O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_Wall_Aabb(),
		Bo1_PFacet_Aabb(),
	],sortThenCollide=True),
	InteractionLoop(
	[
        Ig2_GridNode_GridNode_GridNodeGeom6D(),
		Ig2_Sphere_PFacet_ScGridCoGeom(),
		Ig2_Wall_PFacet_ScGeom(),Ig2_Wall_Sphere_ScGeom()
	],
	[
        Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True),
		Ip2_FrictMat_FrictMat_FrictPhys()],
	[
        Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
		Law2_ScGeom_FrictPhys_CundallStrack(),
		Law2_ScGridCoGeom_FrictPhys_CundallStrack(),
		Law2_GridCoGridCoGeom_FrictPhys_CundallStrack()
	]),
    GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.8,label='ts'), 
	NewtonIntegrator(gravity=(0,-9.81,0),damping=0.1,label='newton')
]


O.materials.append(CohFrictMat(young=1e7,poisson=1,density=1e2,frictionAngle=radians(30),normalCohesion=3e7,shearCohesion=3e7,momentRotationLaw=True,label='gridNodeMat'))
O.materials.append(FrictMat(young=1e7,poisson=1,density=1e2,frictionAngle=radians(30),label='gridConnectionMat'))


###################################
#####   PFacet creators       #####
###################################

fixed = False
color=[255./255.,102./255.,0./255.]

nodesIds=[]
cylIds=[]

#position of the node in the middle
a=0.00
r=0.03


## Option 1: pfacet(id1,id2,id3) -> based on 3 gridNodes already connected via 3 gridConnections

nodesIds.append( O.bodies.append(gridNode([0,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) )
nodesIds.append( O.bodies.append(gridNode([1,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) )
nodesIds.append( O.bodies.append(gridNode([0.5,1,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) )

cylIds.append(O.bodies.append( gridConnection(0,1,r,color=color) ))
cylIds.append(O.bodies.append( gridConnection(1,2,r,color=color) ))
cylIds.append(O.bodies.append( gridConnection(2,0,r,color=color) ))
	
O.bodies.append( pfacet(nodesIds[0],nodesIds[1],nodesIds[2],wire=False,color=color,highlight=False,material=O.materials[1]) )


## Option 2: pfacetCreator1(vertices) -> based on 3 vertices

v1=Vector3(2,0,0)
v2=Vector3(3,0,0)
v3=Vector3(2.5,1,0)
vertices=[v1,v2,v3]
pfacetCreator1(vertices,r,nodesIds=[],cylIds=[],pfIds=[],wire=False,color=color,fixed=fixed,materialNodes='gridNodeMat',material='gridConnectionMat')


## Option 3: pfacetCreator2(id1,id2,vertex) -> based on 2 gridNodes connected via a gridConnection and a vertex

nodesIds.append( O.bodies.append(gridNode([4,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) )
nodesIds.append( O.bodies.append(gridNode([5,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) )
vertex=Vector3(4.5,1,0)
cylIds.append(O.bodies.append( gridConnection(nodesIds[3],nodesIds[4],r,color=color) )	)


pfacetCreator2(nodesIds[3],nodesIds[4],vertex,r,nodesIds=nodesIds,wire=True,materialNodes='gridNodeMat',material='gridConnectionMat',color=color,fixed=fixed)


## Option 4: pfacetCreator3(id1,id2,id3) -> based on 3 gridNodes

a = O.bodies.append(gridNode([6,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
b = O.bodies.append(gridNode([7,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
c = O.bodies.append(gridNode([6.5,1,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
 
pfacetCreator3(a,b,c,cylIds=[],pfIds=[],wire=False,material=-1,color=color)


## Option 4: pfacetCreator4(id1,id2,id3) -> based on 3 gridConnections

a = O.bodies.append(gridNode([8,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
b = O.bodies.append(gridNode([9,0,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
c = O.bodies.append(gridNode([8.5,1,0],r,wire=False,fixed=fixed,material='gridNodeMat',color=color)) 
n=len(cylIds)
cylIds.append(O.bodies.append( gridConnection(a,b,r,color=color) )	)
cylIds.append(O.bodies.append( gridConnection(b,c,r,color=color) )	)
cylIds.append(O.bodies.append( gridConnection(c,a,r,color=color) )	)

pfacetCreator4(cylIds[n],cylIds[n+1],cylIds[n+2],pfIds=[],wire=False,material=-1,color=color)


#####################
#####   Wall      ###
#####################

O.bodies.append(utils.wall(position=-1,sense=0, axis=1,color=Vector3(1,0,0),material='gridConnectionMat'))


qt.View()
O.saveTmp()
