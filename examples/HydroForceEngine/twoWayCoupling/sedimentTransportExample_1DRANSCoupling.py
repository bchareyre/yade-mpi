#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 24/11/2017
#
# Same as sedimentTransportExample but solving a 1D volume averaged fluid momentum balance to determine the fluid velocity profile (i.e. DEM-1D RANS coupling)
# The resolution therefore include a two-way coupling in time between the fluid and the particle behavior, meaning that the fluid is solved every "fluidResolPeriod"
# and account for the presence of particles and for the momentum transfered to the particle phase through the hydrodynamic forces imposed. The fluid-particle system momentum 
# is therefore conserved. 
#
# Data can be saved and plot with the file postProcessing.py
#
############################################################################################################################################################################

#Import libraries
from yade import pack, plot
import math
import random as rand
import numpy as np


##
## Main parameters of the simulation
##

#Particles
diameterPart = 6e-3	#Diameter of the particles, in m
densPart = 2500		#density of the particles, in kg/m3
phiPartMax = 0.61	#Value of the dense packing solid volume fraction, dimensionless
restitCoef = 0.7	#Restitution coefficient of the particles, dimensionless
partFrictAngle = atan(0.5)	#friction angle of the particles, in radian

#Fluid
densFluidPY = 1000.	#Density of the fluid, in kg/m^3
kinematicViscoFluid = 1e-6	#kinematic viscosity of the fluid, in m^2/s
waterDepth = 17.5	#Water depth, in diameter
dtFluid = 1e-5 	#Time step for the fluid resolution, in s
fluidResolPeriod = 1e-2	#Time between two fluid resolution, in s

#Configuration: inclined channel
slope = 0.05	#Inclination angle of the channel slope in radian
lengthCell = 10	#Streamwise length of the periodic cell, in diameter
widthCell = 10	#Spanwise length of the periodic cell, in diameter
Nlayer = 10.	#nb of layer of particle, in diameter
fluidHeight = (Nlayer+waterDepth)*diameterPart	#Height of the flow from the bottom of the sample, in m

saveData = 1	#If put to 1, at each execution of function measure() save the sediment transport rate, fluid velocity, solid volume fraction and velocity profiles for post-processing
endTime = 100	#Time simulated (in seconds)


##
## Secondary parameters of the simulation
##

expoDrag_PY = 3.1	# Richardson Zaki exponent for the hindrance function of the drag force applied to the particles

#Discretization of the sample in ndimz wall-normal (z) steps of size dz, between the bottom of the channel and the position of the water free-surface. Should be equal to the length of the imposed fluid profile. Mesh used for HydroForceEngine.
ndimz = 900	#Number of cells in the height
dz =  fluidHeight/(1.0*(ndimz-1))	# Fluid discretization step in the wall-normal direction	

# Initialization of the main vectors
vxFluidPY = np.zeros(ndimz)	# Vertical fluid velocity profile: u^f = u_x^f(z) e_x, with x the streamwise direction and z the wall-normal
phiPartPY = np.zeros(ndimz)	# Vertical particle volume fraction profile
vxPartPY = np.zeros(ndimz)	# Vertical average particle velocity profile

#Geometrical configuration, define useful quantities
height = 5*fluidHeight	#heigth of the periodic cell, in m (bigger than the fluid height to take into particles jumping above the latter)
length = lengthCell*diameterPart #length of the stream, in m
width = widthCell*diameterPart  #width of the stream, in m
groundPosition = height/4.0 #Definition of the position of the ground, in m
gravityVector = Vector3(9.81*sin(slope),0.0,-9.81*cos(slope)) #Gravity vector to consider a channel inclined with slope angle 'slope'

#Particles contact law/material parameters
maxPressure = (densPart-densFluidPY)*phiPartMax*Nlayer*diameterPart*abs(gravityVector[2]) #Estimated max particle pressure from the static load
normalStiffness = maxPressure*diameterPart*1e4 #Evaluate the minimal normal stiffness to be in the rigid particle limit (cf Roux and Combe 2002)
youngMod = normalStiffness/diameterPart	#Young modulus of the particles from the stiffness wanted.
poissonRatio = 0.5	#poisson's ratio of the particles. Classical values, does not have much influence
O.materials.append(ViscElMat(en=restitCoef, et=0., young=youngMod, poisson=poissonRatio, density=densPart, frictionAngle=partFrictAngle, label='Mat'))  


########################
## FRAMEWORK CREATION ##
########################

#Definition of the semi-periodic cell
O.periodic = True 
O.cell.setBox(length,width,height)

# Reference walls: build two planes at the ground and free-surface to have a reference for the eyes in the 3D view
lowPlane = box(center= (length/2.0, width/2.0,groundPosition),extents=(200,200,0),fixed=True,wire=False,color = (0.,1.,0.),material = 'Mat')
WaterSurface = box(center= (length/2.0, width/2.0,groundPosition+fluidHeight),extents=(2000,width/2.0,0),fixed=True,wire=False,color = (0,0,1),material = 'Mat',mask = 0)
O.bodies.append([lowPlane,WaterSurface]) #add to simulation


# Regular arrangement of spheres sticked at the bottom with random height
L = range(0,int(length/(diameterPart))) #The length is divided in particle diameter
W = range(0,int(width/(diameterPart))) #The width is divided in particle diameter

for x in L: #loop creating a set of sphere sticked at the bottom with a (uniform) random altitude comprised between 0.5 (diameter/12) and 5.5mm (11diameter/12) with steps of 0.5mm. The repartition along z is made around groundPosition.
	for y in W:
		n =  rand.randrange(0,12,1)/12.0*diameterPart     #Define a number between 0 and 11/12 diameter with steps of 1/12 diameter (0.5mm in the experiment)    
		O.bodies.append(sphere((x*diameterPart, y*diameterPart,groundPosition - 11*diameterPart/12.0/2.0 + n),diameterPart/2.,color=(0,0,0),fixed = True,material = 'Mat'))


#Create a loose cloud of particle inside the cell
partCloud = pack.SpherePack()
partVolume = pi/6.*pow(diameterPart,3) #Volume of a particle
partNumber = int(Nlayer*phiPartMax*diameterPart*length*width/partVolume) #Volume of beads to obtain Nlayer layers of particles
partCloud.makeCloud(minCorner=(0,0.,groundPosition+diameterPart),maxCorner=(length,width,groundPosition+fluidHeight),rRelFuzz=0., rMean=diameterPart/2.0, num = partNumber)
partCloud.toSimulation(material='Mat') #Send this packing to simulation with material Mat
#Evaluate the deposition time considering the free-fall time of the highest particle to the ground
depoTime = sqrt(fluidHeight*2/abs(gravityVector[2]))

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
	HydroForceEngine(densFluid = densFluidPY,viscoDyn = kinematicViscoFluid*densFluidPY,zRef = groundPosition,gravity = gravityVector,deltaZ = dz,expoRZ = expoDrag_PY,lift = False,nCell = ndimz,vCell = length*width*dz,radiusPart=diameterPart/2.,vxFluid = np.array(vxFluidPY),phiPart = phiPartPY,vxPart = vxPartPY,ids = idApplyForce, label = 'hydroEngine', dead = True),
	#Solve the fluid volume-averaged 1D momentum balance, RANS 1D
	PyRunner(command = 'fluidModel()', virtPeriod = fluidResolPeriod, label = 'fluidRes', dead = True),
	#Measurement, output files
	PyRunner(command = 'measure()', virtPeriod = 0.1, label = 'measurement', dead = True),
	# Check if the packing is stabilized, if yes activate the hydro force on the grains and the slope.
	PyRunner(command='gravityDeposition(depoTime)',virtPeriod = 0.01,label = 'gravDepo'),
	#GlobalStiffnessTimeStepper, determine the time step
	GlobalStiffnessTimeStepper(defaultDt = 1e-4, viscEl = True,timestepSafetyCoefficient = 0.7,  label = 'GSTS'),
	# Integrate the equation and calculate the new position/velocities...
	NewtonIntegrator(damping=0.2, gravity=gravityVector, label='newtonIntegr')
	]
#save the initial configuration to be able to recharge the simulation starting configuration easily
O.saveTmp()
#run
O.run()

####################################################################################################################################
####################################################  FUNCTION DEFINITION  #########################################################
####################################################################################################################################



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
		hydroEngine.vxFluid = vxFluidPY # Send the fluid velocity vector used to apply the drag fluid force on particles in HydroForceEngine (see c++ code)
		hydroEngine.ReynoldStresses = np.ones(ndimz)*1e-4 # Send the simplified fluid Reynolds stresses Rxz/\rho^f used to account for the fluid velocity fluctuations in HydroForceEngine (see c++ code)
		hydroEngine.turbulentFluctuation() #Initialize the fluid velocity fluctuation associated to particles to zero in HydroForceEngine, necessary to avoid segmentation fault
		measurement.dead = False	# Activate the measure() PyRunner
		fluidRes.dead = False		# Activate the 1D fluid resolution
		hydroEngine.averageProfile()	#Evaluate the solid volume fraction, velocity and drag, necessary for the fluid resolution. 
		hydroEngine.fluidResolution(1.,dtFluid)	#Initialize the fluid resolution, run the fluid resolution for 1s

	return
###############
#########################################


#######			      ########
###	    FLUID RESOLUTION	   ###
#######			      ########
def fluidModel():
	global vxFluidPY,taufsi
	#Evaluate the average vx,vy,vz,phi,drag profiles and store it in hydroEngine, to prepare the fluid resolution
	hydroEngine.averageProfile()
	#Fluid resolution
	hydroEngine.fluidResolution(fluidResolPeriod,dtFluid)	#Solve the fluid momentum balance for a time of fluidResolPeriod s with a time step dtFluid
	#update the fluid velocity for later save
	vxFluidPY = np.array(hydroEngine.vxFluid)
	

#######		      ########
###	    OUTPUT	   ###
#######		      ########
#Initialization
qsMean = 0		#Mean dimensionless sediment transport rate
zAxis = np.zeros(ndimz)	#z scale, in diameter
for i in range(0,ndimz):#z scale used for the possible plot at the end
	zAxis[i] = i*dz/diameterPart

# Averaging/Save
def measure():
	global qsMean,vxPartPY,phiPartPY
	#Evaluate the average depth profile of streamwise, spanwise and wall-normal particle velocity, particle volume fraction (and drag force for coupling with RANS fluid resolution), and store it in hydroEngine variables vxPart, phiPart, vyPart, vzPart, averageDrag.
	hydroEngine.averageProfile()
	#Extract the calculated vector. They can be saved and plotted afterwards. 
	vxPartPY = np.array(hydroEngine.vxPart)
	phiPartPY = np.array(hydroEngine.phiPart)

	#Evaluate the dimensionless sediment transport rate for information
	qsMean = sum(phiPartPY*vxPartPY)*dz/sqrt((densPart/densFluidPY - 1)*abs(gravityVector[2])*pow(diameterPart,3))
	plot.addData(SedimentRate = qsMean, time = O.time)	#Plot it during the simulation

	#Condition to stop the simulation after endTime seconds
	if O.time>=endTime:
		print('\n End of the simulation, simulated {0}s as required !\n '.format(endTime))
		O.pause()

	#Evaluate the Shields number from the maximum of the Reynolds stresses evaluated in the fluid resolution
	shieldsNumber = max(hydroEngine.ReynoldStresses)/((densPart-densFluidPY)*diameterPart*abs(gravityVector[2]))	
	print 'Shields number', shieldsNumber

	if saveData==1:	#Save data for postprocessing
		global fileNumber
		nameFile = scriptPath + '/data/'+ str(fileNumber)+'.py'	# Name of the file that will be saved
		globalParam =  ['qsMean','phiPartPY','vxPartPY','vxFluidPY','zAxis']	# Variables to save
		Save(nameFile, globalParam)	#Save
		fileNumber+=1	#Increment the file number


#Plot the dimensionless sediment transport rate as a function of time during the simulation
plot.plots={'time':('SedimentRate')}
plot.plot()

################
##########################################


#Save data details
fileNumber = 0	# Counter for the file saved
if saveData==1:	#If saveData option is activated, requires a folder data
	scriptPath = os.path.abspath(os.path.dirname(sys.argv[-1])) #Path where the script is stored
	if os.path.exists(scriptPath +'/data/')==False:
		os.mkdir(scriptPath +'/data/')
	else:
		print '\n!! Save data: overwrite the files contains in the folder data/ !!\n'
#Function to save global variables in a python file which can be re-executed for post-processing
def Save(filePathName, globalVariables):
	f = open(filePathName,'w')
	f.write('from numpy import *\n')
	for i in globalVariables:
		f.write(i + ' = '+repr(globals()[i]) + '\n')
	f.close()
