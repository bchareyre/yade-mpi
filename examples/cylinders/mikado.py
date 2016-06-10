# encoding: utf-8
"An example showing how two create cylinders with random length."

from yade.gridpfacet import *

#### Parameters ####
Lmin=3. # minimum length
Lmax=6. # maximum length 
n=10		# number of cylinders in one direction
r=0.5		# radius of the cylinder element
phi=30. # friction angle
E=1e6   # Young's modulus

#### Engines ####
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_GridConnection_Aabb(),
		Bo1_PFacet_Aabb(), 
	]),
	InteractionLoop([
		Ig2_GridNode_GridNode_GridNodeGeom6D(),
		Ig2_GridConnection_GridConnection_GridCoGridCoGeom(),	# cylinder-cylinder interaction
		Ig2_Sphere_PFacet_ScGridCoGeom(),	# needed for GridNode-pFacet interaction (why is this not included in Ig2_GridConnection_PFacet_ScGeom???)
		Ig2_GridConnection_PFacet_ScGeom(),	# Cylinder-pFcet interaction
	],
	[
		Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),
		Ip2_FrictMat_FrictMat_FrictPhys()
	],
	[
		Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),	# contact law for "internal" cylider forces
		Law2_ScGridCoGeom_FrictPhys_CundallStrack(),	# contact law for Cylinder-pFacet interaction
		Law2_GridCoGridCoGeom_FrictPhys_CundallStrack()	# contact law for cylinder-cylinder interaction
	]
	),
	GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.1,label='ts'), 
	NewtonIntegrator(gravity=(0.,0,-10),damping=0.5,label='newton'),
	#NewtonIntegrator(gravity=(-1.,0,-10),damping=0.5,label='newton'),
]

#### Creat materials ####
O.materials.append( CohFrictMat( young=E,poisson=0.3,density=1000,frictionAngle=radians(phi),normalCohesion=1e10,shearCohesion=1e10,momentRotationLaw=True,label='cMat' ) )  # material to create the gridConnections
O.materials.append( FrictMat( young=E,poisson=0.3,density=1000,frictionAngle=radians(phi),label='fMat' ) )  # material for general interactions

#### Create cylinders ####
nodesIds=[]
cylIds=[]
ext=12.
dxy=ext/(n-1)
dL=Lmax-Lmin
random.seed( 10 )
for i in range(0,n):
	y=-ext/2+i*dxy
	for i in range(0,n):
		x=-ext/2+i*dxy
		L=Lmin+dL*random.random()
		color=[random.random(),random.random(),random.random()]
		cylinder((x,y,2*r),(x,y,L+2*r),radius=r,nodesIds=nodesIds,cylIds=cylIds,
					 fixed=False,color=color,intMaterial='cMat',extMaterial='fMat')

#### Creat ground with pFacets ####
color=[255./255.,102./255.,0./255.]
n0=O.bodies.append( gridNode([-10,-10,0],r,wire=False,fixed=True,material='cMat',color=color) )
n1=O.bodies.append( gridNode([10,-10,0],r,wire=False,fixed=True,material='cMat',color=color) )
n2=O.bodies.append( gridNode([10,10,0],r,wire=False,fixed=True,material='cMat',color=color) )
n3=O.bodies.append( gridNode([-10,10,0],r,wire=False,fixed=True,material='cMat',color=color) )
O.bodies.append( gridConnection(n0,n1,r,color=color,material='fMat') )
O.bodies.append( gridConnection(n1,n2,r,color=color,material='fMat') )
O.bodies.append( gridConnection(n2,n0,r,color=color,material='fMat') )
O.bodies.append( gridConnection(n2,n3,r,color=color,material='fMat') )
O.bodies.append( gridConnection(n3,n0,r,color=color,material='fMat') )
O.bodies.append( pfacet(n0,n1,n2,wire=False,material='fMat',color=color) )
O.bodies.append( pfacet(n0,n2,n3,wire=False,material='fMat',color=color) )

#### For viewing ####
from yade import qt
qt.View()
Gl1_Sphere.stripes=True

#### Allows to reload the simulation ####
O.saveTmp()
