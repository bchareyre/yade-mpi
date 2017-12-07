#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 08/07/2016
#
#  Very simplified fluidized bed simulations in order to underline the possibility of HydroForceEngine. 
#  Particles are deposited under gravity inside a box. Once the particles at rest, a constant fluid velocity is applied against gravity, submitting the
#  particles to a constant drag force. Then, a discrete random walk model is applied to mimick the effect of the turbulent fluid velocity fluctuations. 
#  It associates to each particle a random fluid velocity fluctuations in the x, y and z directions, which are taken into account in the evaluation
#  of the drag applied by the fluid on the particle. The intensity of the fluctuation is based on the value of u*, which is imposed through simplifiedReynoldsStress = u*^2
#  The values taken for the fluid velocity and simplifiedReynolds stress have been arbitrarily chosen to have a nice rendering
#
#  The example allows to get familiar with HydroForceEngine and in particular with the included DRW model functions (turbulentFluctuationFluidizedBed() here)
#  For details on the process, it is necessary to have a look to the documentation and the C++ code in pkg/common/ForceEngine.cpp and hpp as HydroForceEngine contains
#  multiple parameters
############################################################################################################################################################################

# ATTENTION: to fit the formulation of HydroForceEngine, the fluid velocity can only be applied along the x axis. Therefore, the gravity is here aligned with x, and all the 
# configuration is defined accordingly. For the 3D visualization, two walls of the cell have been made transparent and clicking on the right xyz button after openning the 
# 3D view allows one to see the sample in the usual way with gravity going from top to bottom. 


#Import libraries
from yade import pack, plot
import math
import random as rand
import numpy as np


##
## Main parameters of the simulation
##

#Particles
diameterPart = 6e-3	#Diameter of the particles, in meter
densPart = 2500		#density of the particles, in kg/m3
restitCoef = 0.8	#Restitution coefficient of the particles
partFrictAngle = atan(0.4)	#friction angle of the particles, in radian

#fluid
densFluidPY = 1.225	#Density of the fluid, (air) in kg/m3
kinematicViscoFluid = 1.48e-5	#kinematic viscosity of the fluid, (air) in m2/s

#Configuration: inclined channel
lengthCell = 100	#length cell along the gravity axis (x), in diameter
widthCell = 10	#length cell along the two other axis, in diameter
Nlayer = 2.5	#nb of layer of particle deposited, in diameter
phiPartMax = 0.61	#Value of the dense packing solid volume fraction

endTime = 10	#Time simulated (in seconds)


##
## Secondary parameters of the simulation
##

expoDrag_PY = 3.1	# Richardson Zaki exponent for the hindrance function of the drag force applied to the particles
ndimz = 20	#Number of cells in the height
dz =  widthCell*diameterPart/ndimz	# Fluid discretization step in the wall-normal direction	

# Initialization of the main vectors
vxFluidPY = np.ones(ndimz)*18.5	# Vertical fluid velocity profile: u^f = u_x^f(z) e_x, with x the streamwise direction and z the wall-normal

phiPartPY = np.zeros(ndimz)	# Vertical particle volume fraction profile
vxPartPY = np.zeros(ndimz)	# Vertical average particle velocity profile

#Geometrical configuration, define useful quantities
length = lengthCell*diameterPart #length of the stream, in m
width = widthCell*diameterPart  #width of the stream, in m
groundPosition = 0. #Definition of the position of the ground, in m
gravityVector = Vector3(-9.81,0.0,0.) #Gravity vector. Inclined along x to have an effect opposed to the fluid flow (which can only be along x)

#Particles contact law/material parameters
maxPressure = (densPart-densFluidPY)*phiPartMax*Nlayer*diameterPart*9.81#Estimated max particle pressure from the static load
normalStiffness = maxPressure*diameterPart*1e4 #Evaluate the minimal normal stiffness to be in the rigid particle limit (cf Roux and Combe 2002)
youngMod = normalStiffness/diameterPart	#Young modulus of the particles from the stiffness wanted.
poissonRatio = 0.5	#poisson's ratio of the particles. Classical values, does not have much influence
O.materials.append(ViscElMat(en=restitCoef, et=0., young=youngMod, poisson=poissonRatio, density=densPart, frictionAngle=partFrictAngle, label='Mat'))  


########################
## FRAMEWORK CREATION ##
########################

# Walls to create a box
lowPlane = box(center= (groundPosition, width/2.0,width/2.),extents=(0,width/2.,width/2.),fixed=True,wire=False,color = (0.,1.,0.),material = 'Mat')
sidePlane1 = box(center= (length/2., width,width/2.),extents=(length/2.,0.,width/2.),fixed=True,wire=True,color = (0.,1.,0.),material = 'Mat')
sidePlane2 = box(center= (length/2., 0.,width/2.),extents=(length/2.,0.,width/2.),fixed=True,wire=True,color = (0.,1.,0.),material = 'Mat')
sidePlane3 = box(center= (length/2.,width/2., 0.),extents=(length/2.,width/2.,0.),fixed=True,wire=False,color = (0.,1.,0.),material = 'Mat')	#Made invisible (wire = True) in order to see inside the cell
sidePlane4 = box(center= (length/2.,width/2., width),extents=(length/2.,width/2.,0.),fixed=True,wire=False,color = (0.,1.,0.),material = 'Mat')	#Made invisible (wire = True) in order to see inside the cell
O.bodies.append([lowPlane,sidePlane1,sidePlane2,sidePlane3,sidePlane4])

#Create a loose cloud of particle inside the cell
partCloud = pack.SpherePack()
partVolume = pi/6.*pow(diameterPart,3) #Volume of a particle
partNumber = int(Nlayer*phiPartMax*diameterPart*width*width/partVolume) #Volume of beads to obtain Nlayer layers of particles
partCloud.makeCloud(minCorner=(0,0.,0),maxCorner=(length,width,width),rRelFuzz=0., rMean=diameterPart/2.0, num = partNumber)
partCloud.toSimulation(material='Mat') #Send this packing to simulation with material Mat
#Evaluate the deposition time considering the free-fall time of the highest particle to the ground
depoTime = sqrt(length*2/9.31)

# Collect the ids of the spheres which are dynamic to add a fluid force through HydroForceEngines
idApplyForce = []
for b in O.bodies: 
	if isinstance(b.shape,Sphere) and b.dynamic:
		idApplyForce+=[b.id]


#########################
#### SIMULATION LOOP#####
#########################

O.engines = [
	# Reset the forces
	ForceResetter(),
	# Detect the potential contacts
	InsertionSortCollider([Bo1_Sphere_Aabb(), Bo1_Wall_Aabb(),Bo1_Facet_Aabb(),Bo1_Box_Aabb()],label='contactDetection',allowBiggerThanPeriod = True),
	# Calculate the different interactions
	InteractionLoop(
   	[Ig2_Sphere_Sphere_ScGeom(), Ig2_Box_Sphere_ScGeom()],
   	[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
   	[Law2_ScGeom_ViscElPhys_Basic()]
	,label = 'interactionLoop'),				
	#Apply an hydrodynamic force to the particles
	HydroForceEngine(densFluid = densFluidPY,viscoDyn = kinematicViscoFluid*densFluidPY,zRef = groundPosition,gravity = gravityVector,deltaZ = dz,expoRZ = expoDrag_PY,lift = False,nCell = ndimz,vCell = length*width*dz ,vxFluid = vxFluidPY,phiPart = phiPartPY,vxPart = vxPartPY,ids = idApplyForce,vFluctX = np.zeros(len(O.bodies)),vFluctY = np.zeros(len(O.bodies)),vFluctZ = np.zeros(len(O.bodies)), label = 'hydroEngine', dead = True),
	#Measurement, output files
	PyRunner(command = 'measure()', virtPeriod = 0.1, label = 'measurement', dead = True),
	# Check if the packing is stabilized, if yes activate the hydro force on the grains and the slope.
	PyRunner(command='gravityDeposition(depoTime)',virtPeriod = 0.01,label = 'gravDepo'),
	#Apply fluid turbulent fluctuations from a discrete random walk model
	PyRunner(command='turbulentFluctuations()',virtPeriod = 0.01,label = 'turbFluct'),
	#GlobalStiffnessTimeStepper, determine the time step
	GlobalStiffnessTimeStepper(defaultDt = 1e-4, viscEl = True,timestepSafetyCoefficient = 0.7,  label = 'GSTS'),
	# Integrate the equation and calculate the new position/velocities...
	NewtonIntegrator(damping=0.2, gravity=gravityVector, label='newtonIntegr')
	]
#save the initial configuration to be able to recharge the simulation starting configuration easily
O.saveTmp()
#run
#O.run()
#####################################################################################################################################
#####################################################  FUNCTION DEFINITION  #########################################################
#####################################################################################################################################



######								           ######
### LET THE TIME FOR THE GRAVITY DEPOSITION AND ACTIVATE THE FLUID AT THE END ###
######								           ######
def gravityDeposition(lim):
	if O.time<lim : return
	else :		
		print('\n Gravity deposition finished, apply fluid forces !\n')
		newtonIntegr.damping = 0.0	# Set the artificial numerical damping to zero
	   	gravDepo.dead = True	# Remove the present engine for the following
		hydroEngine.dead = False	# Activate the HydroForceEngine
		turbFluct.dead = False	# Activate the turbulent fluctuations model
	return
###############
#########################################


######							       		           ######
###   APPLY TURBULENT FLUID VELOCITY FLUCTUATIONS FROM A DISCRETE RANDOM WALK MODEL   ###
# Associate a random fluctuation to each particles from the value of the Reynolds 	#
# 	stresses Rxz imposed in simplifiedReynoldsStress = Rxz/densFluid		#
#Details in the C++ code: function turbulentFluctuationsFluidizedBed in ForceEngine.cpp #
######								         	   ######
hydroEngine.simplifiedReynoldStresses = np.ones(ndimz)*4.
def turbulentFluctuations():
	hydroEngine.turbulentFluctuationFluidizedBed()


