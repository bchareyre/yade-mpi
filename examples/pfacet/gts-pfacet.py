from yade import qt
from yade.gridpfacet import *
import gts, os.path, locale

locale.setlocale(locale.LC_ALL, 'en_US.UTF-8')   # Note: gts is locale-dependend. If, for example, german locale is used, gts.read()-function does not import floats normally

'''
if you get "Error: unsupported locale setting"
-> type as root: "dpkg-reconfigure locales"
-> choose "en_US.UTF-8" (press space to choose)
'''

################
### ENGINES  ###
################

O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Wall_Aabb(),
		Bo1_PFacet_Aabb(),
	],sortThenCollide=True),
	InteractionLoop(
	[
        Ig2_GridNode_GridNode_GridNodeGeom6D(),
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
    GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.5,label='ts'), 
	NewtonIntegrator(gravity=(0,-9.81,0),damping=0.1,label='newton')
]


#################
### MATERIAL  ###
#################

O.materials.append(CohFrictMat(young=1e8,poisson=0.3,density=2650,frictionAngle=radians(20),normalCohesion=1e100,shearCohesion=1e100,momentRotationLaw=True,label='gridNodeMat'))
O.materials.append(FrictMat(young=1e8,poisson=0.3,density=2650,frictionAngle=radians(20),label='pFacetMat'))


###################
### IMPORT MESH ###
###################
radius=1e-02
wire=False
fixed=False

z=-1.2
color=[0,0,1]

nodesIds0,cylIds0,pfIds0 =  gtsPFacet('octahedron.gts',shift=(0,0,0),scale=1.,radius=radius,wire=wire,fixed=fixed,materialNodes='gridNodeMat',material='pFacetMat',color=color)

nodesIds1,cylIds1,pfIds1 =  gtsPFacet('box.gts',shift=(3.,0.,0.),scale=2,radius=radius,wire=wire,fixed=fixed,materialNodes='gridNodeMat',material='pFacetMat',color=color)

nodesIds2,cylIds2,pfIds2 =  gtsPFacet('sphere.gts',shift=(6.,0.,0.),scale=1.0,radius=radius,wire=wire,fixed=fixed,materialNodes='gridNodeMat',material='pFacetMat',color=color)



#####################
#####   Wall      ###
#####################

O.bodies.append(utils.wall(position=z,sense=0, axis=1,color=Vector3(1,0,0),material='pFacetMat'))


##########
## VIEW ##
##########

qt.Controller()
qtv = qt.View()
qtr = qt.Renderer()
qtr.light2=True
qtr.lightPos=Vector3(1200,1500,500)
qtr.bgColor=[1,1,1]
qtv.ortho=True


O.saveTmp()
