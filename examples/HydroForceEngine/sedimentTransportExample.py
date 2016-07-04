#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 04/07/2016
#
# Example script to simulate sediment transport from an imposed 1D vertical fluid profile
# Make use of HydroForceEngine, which is tailored to be used with a 1D RANS code, solving the fluid momentum balance as a function of the depth to determine u_x^f(z) the vertical fluid profile (vxFluidPY).
# 
# Create a bi-periodic inclined channel with a rough bottom, and deposit a cloud of particles under gravity to form a granular bed. 
# Apply fluid forces from an imposed fluid profile vxFluidPY through the engine HydroForceEngine
# Let evolve the problem to equilibrium and measure the granular depth profiles of particles volume fraction and particle velocity
#
# The force applied by the fluid on the particles are restricted to drag and buoyancy. To know the exact expressions of the latter (and for any precisions regarding an action of HydroForceEngine), please  refer to the documentation of HydroForceEngine and to the c++ source code pkg/common/ForceEngine.cpp and hpp. 
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
diameterPart = 6e-3	#Diameter of the particles, in meter
densPart = 2500		#density of the particles, in kg/m3
phiPartMax = 0.61	#Value of the dense packing solid volume fraction
restitCoef = 0.5	#Restitution coefficient of the particles
partFrictAngle = atan(0.4)	#friction angle of the particles, in radian

#fluid
densFluidPY = 1000.	#Density of the fluid
kinematicViscoFluid = 1e-6	#kinematic viscosity of the fluid
waterDepth = 20.#Water depth in diameter

#Configuration: inclined channel
slope = 0.05	#Inclination angle of the channel slope in radian
lengthCell = 10	#Streamwise length of the periodic cell, in diameter
widthCell = 10	#Spanwise length of the periodic cell, in diameter
Nlayer = 10.	#nb of layer of particle, in diameter
fluidHeight = (Nlayer+waterDepth)*diameterPart	#Height of the flow from the bottom of the sample

endTime = 10	#Time simulated (in seconds)



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
gravityVectorApplied = Vector3(0.0,0.0,-9.81*cos(slope)) #Applied gravity for buoyancy (no x contribution in turbulent cases)

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
	HydroForceEngine(densFluid = densFluidPY,viscoDyn = kinematicViscoFluid*densFluidPY,zRef = groundPosition,gravity = gravityVectorApplied,deltaZ = dz,expoRZ = expoDrag_PY,lift = False,nCell = ndimz,vCell = length*width*dz ,vxFluid = vxFluidPY,phiPart = phiPartPY,vxPart = vxPartPY,ids = idApplyForce, label = 'hydroEngine', dead = True),
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
		hydroEngine.simplifiedReynoldStresses = np.ones(ndimz)*1e-4 # Send the simplified fluid Reynolds stresses Rxz/\rho^f used to account for the fluid velocity fluctuations in HydroForceEngine (see c++ code)
		hydroEngine.turbulentFluctuation() #Initialize the fluid velocity fluctuation associated to particles to zero in HydroForceEngine, necessary to avoid segmentation fault

		measurement.dead = False	# Activate the measure() PyRunner
	return
###############
#########################################



#######		      ########
###	    OUTPUT	   ###
#######		      ########
# Averaging/Save
qsMean = 0		#Mean dimensionless sediment transport rate
zAxis = np.zeros(ndimz)	#z scale, in diameter
def measure():
	global qsMean
	global vxPartPY
	global phiPartPY
	global zAxis
	#Evaluate the average depth profile of streamwise, spanwise and wall-normal particle velocity, particle volume fraction (and drag force for coupling with RANS fluid resolution), and store it in hydroEngine variables vxPart, vyPart, vzPart, phiPart, averageDrag
	hydroEngine.averageProfile()
	#Extract the calculated vector. They can be saved and plotted afterwards. 
	vxPartPY = np.array(hydroEngine.vxPart)
	vyPartPY = np.array(hydroEngine.vyPart)
	vzPartPY = np.array(hydroEngine.vzPart)
	phiPartPY = np.array(hydroEngine.phiPart)
	averageDragPY = np.array(hydroEngine.averageDrag)

	#Evaluate the dimensionless sediment transport rate for information
	qsMean = sum(phiPartPY*vxPartPY)*dz/sqrt((densPart/densFluidPY - 1)*abs(gravityVector[2])*pow(diameterPart,3))
	plot.addData(SedimentRate = qsMean, time = O.time)	#Plot it during the simulation

	#Condition to stop the simulation after endTime seconds
	if O.time>=endTime:
		print('\n End of the simulation, simulated {0}s as required !\n '.format(endTime))
		O.pause()

		#Z scale used for the possible plot at the end
		global zAxis
		for i in range(0,ndimz):
			zAxis[i] = i*dz/diameterPart

## Paste the following (uncommented) code in the console at the end of the simulation to observe the shape of the granular depth profiles (solid volume fraction and velocity)
#import matplotlib.pyplot as pyp
#pyp.figure('solid volume fraction profile')
#pyp.plot(phiPartPY,zAxis,'g')
#pyp.xlabel(r'$\phi$')
#pyp.ylabel(r'$z/d$')
#pyp.figure('streamwise particle velocity profile')
#pyp.plot(vxPartPY,zAxis,'g')
#pyp.xlabel(r'$<v_x^p>$')
#pyp.ylabel(r'$z/d$')
#pyp.show()

#Plot the dimensionless sediment transport rate as a function of time during the simulation
plot.plots={'time':('SedimentRate')}
plot.plot()

################
##########################################


#Imposed fluid profile, corresponding to a Shields number 0.5
vxFluidPY = np.array([ 0.        ,  0.06156562,  0.08728988,  0.10283756,  0.11005262,
        0.11059876,  0.1060113 ,  0.0976961 ,  0.08701183,  0.07527384,
        0.06375715,  0.05362684,  0.04587625,  0.04124045,  0.04014932,
        0.04265518,  0.04835575,  0.05637364,  0.06555057,  0.07466258,
        0.0825892 ,  0.08848402,  0.09180331,  0.09234952,  0.09026192,
        0.08598644,  0.08023121,  0.07377648,  0.0673878 ,  0.06174271,
        0.05740186,  0.0547775 ,  0.05407727,  0.05527346,  0.05813167,
        0.0622459 ,  0.06708855,  0.07204081,  0.07651257,  0.07999516,
        0.08215522,  0.08283083,  0.08204372,  0.07996273,  0.07689339,
        0.0732465 ,  0.06949481,  0.06610862,  0.06347541,  0.06185502,
        0.0613583 ,  0.06194208,  0.06343303,  0.0655741 ,  0.06804706,
        0.07051492,  0.0726754 ,  0.07429343,  0.07521449,  0.07537433,
        0.07479194,  0.07357723,  0.07192908,  0.07010514,  0.06839215,
        0.06704851,  0.06626627,  0.06614762,  0.06669344,  0.06779834,
        0.06927958,  0.07091005,  0.07245458,  0.07369861,  0.07446029,
        0.07462772,  0.07419583,  0.07325704,  0.07196604,  0.07049793,
        0.06903213,  0.06774085,  0.06677561,  0.0662449 ,  0.06619538,
        0.06661213,  0.06742787,  0.0685343 ,  0.06978656,  0.07102537,
        0.07210075,  0.07289378,  0.07334036,  0.07343889,  0.07323677,
        0.07280684,  0.0722337 ,  0.07160383,  0.07099545,  0.07047215,
        0.07007524,  0.06982274,  0.06971082,  0.06971833,  0.06980796,
        0.06993805,  0.07007409,  0.07019433,  0.07028775,  0.07034303,
        0.07035078,  0.07031745,  0.07026673,  0.07022685,  0.070222  ,
        0.07026871,  0.07037337,  0.0705376 ,  0.07075427,  0.07100814,
        0.07128164,  0.07155703,  0.07182318,  0.07207437,  0.07230019,
        0.0724788 ,  0.0725801 ,  0.07257863,  0.07245956,  0.07222669,
        0.07189895,  0.07150785,  0.07108934,  0.07068479,  0.07033428,
        0.0700743 ,  0.06993614,  0.06994256,  0.07010162,  0.07040221,
        0.07081663,  0.07130815,  0.07183653,  0.07236057,  0.0728381 ,
        0.07323606,  0.07353095,  0.07371097,  0.0737761 ,  0.07373514,
        0.07360549,  0.073415  ,  0.07319872,  0.07299079,  0.07282058,
        0.07271034,  0.07267235,  0.07270862,  0.07281215,  0.07297214,
        0.07317416,  0.07340123,  0.07363392,  0.07385621,  0.07405428,
        0.07421823,  0.07434827,  0.07445663,  0.07456325,  0.07469148,
        0.07486482,  0.07510363,  0.0754249 ,  0.07583499,  0.0763238 ,
        0.07687   ,  0.07744048,  0.07799326,  0.07848547,  0.07887646,
        0.07913544,  0.07924846,  0.07921931,  0.07907331,  0.07885561,
        0.07862464,  0.07844374,  0.07837516,  0.07847163,  0.0787705 ,
        0.07928899,  0.08002083,  0.0809382 ,  0.08199479,  0.08313004,
        0.08427904,  0.08537916,  0.08637918,  0.08724493,  0.08796148,
        0.08853075,  0.08897183,  0.08931631,  0.0896013 ,  0.08986449,
        0.09013922,  0.09045455,  0.09083446,  0.0912941 ,  0.09184645,
        0.09249791,  0.09325393,  0.09411605,  0.0950802 ,  0.09613639,
        0.09727042,  0.09846512,  0.0997008 ,  0.10095958,  0.10222823,
        0.10349927,  0.10477299,  0.10605354,  0.10735303,  0.10868852,
        0.11007881,  0.11154475,  0.11310325,  0.11476599,  0.11653497,
        0.118402  ,  0.12034988,  0.12235397,  0.12438468,  0.12641134,
        0.12840556,  0.13034169,  0.13220338,  0.13398571,  0.13570116,
        0.13738374,  0.13909178,  0.1409052 ,  0.14291927,  0.14523157,
        0.14793003,  0.1510891 ,  0.15476738,  0.15900948,  0.16384828,
        0.16930529,  0.17539348,  0.18211907,  0.1894816 ,  0.19747537,
        0.20609019,  0.2153121 ,  0.22512414,  0.23550562,  0.24643343,
        0.25788275,  0.26982699,  0.28223862,  0.29508925,  0.3083499 ,
        0.32199109,  0.33598346,  0.35029802,  0.36490592,  0.37977898,
        0.39488962,  0.41021114,  0.42571807,  0.4413862 ,  0.45719298,
        0.47311751,  0.4891401 ,  0.50524264,  0.52140786,  0.53761979,
        0.55386412,  0.57012801,  0.58639949,  0.60266791,  0.61892388,
        0.63515886,  0.65136517,  0.66753551,  0.68366317,  0.69974232,
        0.71576767,  0.73173404,  0.74763662,  0.76347108,  0.77923328,
        0.79491932,  0.81052539,  0.82604798,  0.8414839 ,  0.85683046,
        0.87208498,  0.88724521,  0.90230903,  0.91727463,  0.93214057,
        0.94690566,  0.96156893,  0.97612958,  0.99058652,  1.00493896,
        1.0191864 ,  1.03332831,  1.04736448,  1.06129493,  1.07511968,
        1.08883882,  1.10245244,  1.11596073,  1.1293639 ,  1.14266233,
        1.15585634,  1.16894621,  1.18193214,  1.19481443,  1.2075934 ,
        1.22026932,  1.23284246,  1.24531294,  1.257681  ,  1.26994706,
        1.2821116 ,  1.29417505,  1.30613785,  1.31800031,  1.32976287,
        1.34142608,  1.3529905 ,  1.36445665,  1.37582494,  1.38709576,
        1.39826949,  1.40934651,  1.42032734,  1.43121251,  1.44200244,
        1.45269741,  1.46329784,  1.47380421,  1.4842169 ,  1.4945363 ,
        1.50476284,  1.51489687,  1.52493875,  1.53488882,  1.5447474 ,
        1.55451477,  1.56419123,  1.57377699,  1.58327231,  1.59267751,
        1.60199298,  1.61121897,  1.62035577,  1.6294036 ,  1.63836267,
        1.64723331,  1.65601585,  1.66471064,  1.67331804,  1.68183842,
        1.69027212,  1.6986195 ,  1.70688092,  1.71505672,  1.72314731,
        1.73115306,  1.73907436,  1.74691168,  1.75466545,  1.76233612,
        1.76992419,  1.77743015,  1.78485456,  1.79219798,  1.79946097,
        1.80664408,  1.81374788,  1.82077303,  1.82772023,  1.83459016,
        1.84138355,  1.84810114,  1.85474367,  1.8613119 ,  1.86780655,
        1.87422842,  1.88057834,  1.88685715,  1.89306574,  1.89920502,
        1.90527587,  1.9112792 ,  1.91721589,  1.92308686,  1.92889305,
        1.9346354 ,  1.94031485,  1.94593232,  1.95148872,  1.956985  ,
        1.96242207,  1.96780085,  1.97312225,  1.97838716,  1.9835965 ,
        1.98875115,  1.99385197,  1.99889988,  2.00389572,  2.00884037,
        2.01373468,  2.01857948,  2.02337561,  2.02812387,  2.03282505,
        2.03747992,  2.04208926,  2.04665378,  2.05117424,  2.05565134,
        2.06008578,  2.06447826,  2.06882943,  2.07313998,  2.07741053,
        2.08164173,  2.08583421,  2.08998857,  2.09410541,  2.09818531,
        2.10222885,  2.10623657,  2.11020902,  2.11414674,  2.11805024,
        2.12192005,  2.12575664,  2.12956052,  2.13333217,  2.13707204,
        2.14078061,  2.14445833,  2.14810563,  2.15172294,  2.1553107 ,
        2.1588693 ,  2.16239916,  2.16590066,  2.1693742 ,  2.17282014,
        2.17623887,  2.17963073,  2.18299609,  2.1863353 ,  2.18964869,
        2.19293659,  2.19619934,  2.19943724,  2.20265063,  2.2058398 ,
        2.20900506,  2.2121467 ,  2.21526502,  2.21836029,  2.22143281,
        2.22448284,  2.22751065,  2.23051652,  2.23350069,  2.23646343,
        2.23940498,  2.2423256 ,  2.24522553,  2.248105  ,  2.25096425,
        2.25380351,  2.256623  ,  2.25942296,  2.26220359,  2.26496512,
        2.26770775,  2.27043169,  2.27313715,  2.27582432,  2.2784934 ,
        2.2811446 ,  2.28377809,  2.28639406,  2.28899271,  2.2915742 ,
        2.29413873,  2.29668646,  2.29921756,  2.30173222,  2.30423059,
        2.30671284,  2.30917913,  2.31162963,  2.31406448,  2.31648385,
        2.31888789,  2.32127674,  2.32365056,  2.32600949,  2.32835368,
        2.33068326,  2.33299837,  2.33529916,  2.33758576,  2.33985829,
        2.34211691,  2.34436172,  2.34659286,  2.34881046,  2.35101464,
        2.35320552,  2.35538322,  2.35754787,  2.35969957,  2.36183844,
        2.3639646 ,  2.36607817,  2.36817924,  2.37026793,  2.37234435,
        2.3744086 ,  2.37646079,  2.37850103,  2.3805294 ,  2.38254602,
        2.38455099,  2.38654439,  2.38852634,  2.39049692,  2.39245622,
        2.39440435,  2.39634139,  2.39826743,  2.40018257,  2.40208688,
        2.40398047,  2.40586341,  2.40773578,  2.40959768,  2.41144918,
        2.41329037,  2.41512133,  2.41694214,  2.41875287,  2.4205536 ,
        2.42234442,  2.42412539,  2.42589659,  2.4276581 ,  2.42940998,
        2.43115232,  2.43288518,  2.43460863,  2.43632274,  2.43802758,
        2.43972322,  2.44140973,  2.44308716,  2.4447556 ,  2.4464151 ,
        2.44806572,  2.44970754,  2.4513406 ,  2.45296499,  2.45458074,
        2.45618793,  2.45778662,  2.45937686,  2.46095872,  2.46253225,
        2.4640975 ,  2.46565454,  2.46720342,  2.46874419,  2.47027691,
        2.47180164,  2.47331842,  2.47482732,  2.47632837,  2.47782164,
        2.47930717,  2.48078502,  2.48225523,  2.48371786,  2.48517295,
        2.48662055,  2.48806071,  2.48949348,  2.4909189 ,  2.49233702,
        2.49374788,  2.49515154,  2.49654803,  2.4979374 ,  2.49931969,
        2.50069495,  2.50206322,  2.50342454,  2.50477896,  2.50612651,
        2.50746724,  2.50880118,  2.51012839,  2.51144889,  2.51276272,
        2.51406994,  2.51537057,  2.51666465,  2.51795223,  2.51923333,
        2.520508  ,  2.52177627,  2.52303818,  2.52429376,  2.52554305,
        2.52678609,  2.52802291,  2.52925355,  2.53047803,  2.5316964 ,
        2.53290869,  2.53411492,  2.53531514,  2.53650937,  2.53769765,
        2.53888   ,  2.54005647,  2.54122708,  2.54239187,  2.54355086,
        2.54470408,  2.54585156,  2.54699334,  2.54812944,  2.5492599 ,
        2.55038473,  2.55150398,  2.55261766,  2.55372581,  2.55482845,
        2.55592562,  2.55701733,  2.55810362,  2.55918451,  2.56026002,
        2.5613302 ,  2.56239505,  2.56345461,  2.5645089 ,  2.56555795,
        2.56660178,  2.56764042,  2.56867388,  2.56970221,  2.57072541,
        2.57174351,  2.57275654,  2.57376452,  2.57476747,  2.57576542,
        2.57675838,  2.57774639,  2.57872946,  2.57970761,  2.58068087,
        2.58164925,  2.58261279,  2.58357149,  2.58452539,  2.5854745 ,
        2.58641884,  2.58735843,  2.58829329,  2.58922345,  2.59014892,
        2.59106972,  2.59198587,  2.59289739,  2.5938043 ,  2.59470662,
        2.59560436,  2.59649755,  2.5973862 ,  2.59827033,  2.59914996,
        2.60002511,  2.60089579,  2.60176202,  2.60262382,  2.6034812 ,
        2.60433419,  2.60518279,  2.60602703,  2.60686692,  2.60770247,
        2.60853371,  2.60936065,  2.6101833 ,  2.61100168,  2.6118158 ,
        2.61262569,  2.61343135,  2.6142328 ,  2.61503005,  2.61582313,
        2.61661203,  2.61739678,  2.6181774 ,  2.61895388,  2.61972626,
        2.62049454,  2.62125873,  2.62201885,  2.62277492,  2.62352694,
        2.62427492,  2.62501889,  2.62575884,  2.62649481,  2.62722678,
        2.62795479,  2.62867884,  2.62939894,  2.6301151 ,  2.63082734,
        2.63153566,  2.63224009,  2.63294062,  2.63363727,  2.63433004,
        2.63501896,  2.63570404,  2.63638527,  2.63706267,  2.63773625,
        2.63840603,  2.639072  ,  2.63973419,  2.64039259,  2.64104723,
        2.6416981 ,  2.64234522,  2.64298859,  2.64362823,  2.64426414,
        2.64489633,  2.64552481,  2.64614958,  2.64677067,  2.64738806,
        2.64800178,  2.64861182,  2.6492182 ,  2.64982092,  2.65041999,
        2.65101542,  2.6516072 ,  2.65219536,  2.6527799 ,  2.65336081,
        2.65393812,  2.65451182,  2.65508192,  2.65564842,  2.65621134,
        2.65677067,  2.65732642,  2.6578786 ,  2.65842722,  2.65897226,
        2.65951375,  2.66005168,  2.66058606,  2.6611169 ,  2.66164419,
        2.66216794,  2.66268815,  2.66320484,  2.66371799,  2.66422761,
        2.66473371,  2.66523629,  2.66573534,  2.66623088,  2.66672291,
        2.66721142,  2.66769641,  2.6681779 ,  2.66865588,  2.66913034,
        2.6696013 ,  2.67006875,  2.67053269,  2.67099312,  2.67145005,
        2.67190346,  2.67235337,  2.67279976,  2.67324264,  2.67368201,
        2.67411786,  2.6745502 ,  2.67497901,  2.6754043 ,  2.67582607,
        2.67624431,  2.67665901,  2.67707018,  2.6774778 ,  2.67788188,
        2.67828241,  2.67867939,  2.6790728 ,  2.67946264,  2.67984891,
        2.6802316 ,  2.6806107 ,  2.68098621,  2.68135811,  2.6817264 ,
        2.68209106,  2.6824521 ,  2.68280949,  2.68316323,  2.6835133 ,
        2.6838597 ,  2.68420241,  2.68454142,  2.68487671,  2.68520828,
        2.6855361 ,  2.68586015,  2.68618044,  2.68649692,  2.68680959,
        2.68711843,  2.68742342,  2.68772453,  2.68802174,  2.68831504,
        2.68860439,  2.68888977,  2.68917115,  2.6894485 ,  2.6897218 ,
        2.689991  ,  2.69025609,  2.69051701,  2.69077374,  2.69102624,
        2.69127446,  2.69151836,  2.69175789,  2.69199301,  2.69222365,
        2.69244977,  2.6926713 ,  2.69288819,  2.69310035,  2.69330773,
        2.69351023,  2.69370778,  2.69390028,  2.69408764,  2.69426974,
        2.69444647,  2.6946177 ,  2.69478329,  2.69494307,  2.69509687,
        2.69524449,  2.69538571,  2.69552028,  2.69564788,  2.69576818,
        2.69588076,  2.6959851 ,  2.69608056,  2.6961663 ,  2.69624114,
        2.6963033 ,  2.69634954,  2.69637016,  2.69633447,  2.69633447])

if ndimz!=len(vxFluidPY):
	print '\n Bug: ndimz should necessarily be equal to the length of the imposed fluid profile vxFluidPY !\n'
	exit()

