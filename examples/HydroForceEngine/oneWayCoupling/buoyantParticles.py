#########################################################################################################################################################################
# Author: Raphael Maurin, raphael.maurin@imft.fr
# 07/07/2016
#
# Example script to use HydroForceEngine in order to apply buoyancy force to particles. 
# The fluid is supposed at rest (vxFluidPY is a zero vector) so that the particles are only submitted to a drag force opposing the motion of the particle. 
# Three spheres with density 1500, 1000 and 500kg/m3 are positionned at the middle of a fluid sample of density 1000 kg/m3, and let evolved with time
# We observe clearly that one sphere sediment down to the bottom, another one is rising to the top of the water free-surface and the third one does not move
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
restitCoef = 0.5	#Restitution coefficient of the particles
partFrictAngle = atan(0.4)	#friction angle of the particles, in radian
densPart1 = 1050		#density of the particles, in kg/m3
densPart2 = 1000		#density of the particles, in kg/m3
densPart3 = 950		#density of the particles, in kg/m3

#fluid
densFluidPY = 1000.	#Density of the fluid
kinematicViscoFluid = 1e-6	#kinematic viscosity of the fluid
fluidHeight = 20.*diameterPart	#Water depth in m.
ndimz = 1
groundPosition = 0.#Definition of the position of the ground, in m
gravityVector = Vector3(0,0.0,-9.81) #Gravity vector

#Particles contact law/material parameters
normalStiffness = 1e4
youngMod = normalStiffness/diameterPart	#Young modulus of the particles from the stiffness wanted.
poissonRatio = 0.5	#poisson's ratio of the particles. Classical values, does not have much influence

#Material of particle 1, 2, and 3, with different density densPart1, densPart2 and densPart3 defined above (1500, 1000 and 500kg/m3 by default) 
O.materials.append(ViscElMat(en=restitCoef, et=0., young=youngMod, poisson=poissonRatio, density=densPart1, frictionAngle=partFrictAngle, label='Mat1'))  
O.materials.append(ViscElMat(en=restitCoef, et=0., young=youngMod, poisson=poissonRatio, density=densPart2, frictionAngle=partFrictAngle, label='Mat2'))  
O.materials.append(ViscElMat(en=restitCoef, et=0., young=youngMod, poisson=poissonRatio, density=densPart3, frictionAngle=partFrictAngle, label='Mat3'))  

#Time of simulation
endTime = 2.

########################
## FRAMEWORK CREATION ##
########################

# Reference walls: build a wall at the ground and draw the position of the free-surface to have a reference for the eyes in the 3D view
lowPlane = box(center= (0, 0,groundPosition),extents=(200,200,0),fixed=True,wire=False,color = (0.,1.,0.))
WaterSurface = box(center= (0,0,groundPosition+fluidHeight),extents=(200,200,0),fixed=True,wire=False,color = (0,0,1),mask = 0)
O.bodies.append([lowPlane,WaterSurface]) #add to simulation

id1 = O.bodies.append(sphere(center = (0,2*diameterPart,groundPosition + fluidHeight/2.), radius = diameterPart/2.,material = 'Mat1'))
id2 = O.bodies.append(sphere(center = (0,0,groundPosition + fluidHeight/2.), radius = diameterPart/2.,material = 'Mat2'))
id3 = O.bodies.append(sphere(center = (0,-2*diameterPart,groundPosition + fluidHeight/2.), radius = diameterPart/2.,material = 'Mat3'))

# Collect the ids of the spheres which are dynamic to add a fluid force through HydroForceEngines
idApplyForce = [id1,id2,id3]


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
	HydroForceEngine(densFluid = densFluidPY,viscoDyn = kinematicViscoFluid*densFluidPY,zRef = groundPosition,gravity = gravityVector,deltaZ = fluidHeight/ndimz,expoRZ = 0.,lift = False,nCell = ndimz,vCell = 1.,vxFluid = np.zeros(ndimz),phiPart = np.zeros(ndimz),vxPart = np.zeros(ndimz),vFluctX = np.zeros(len(O.bodies)),vFluctY = np.zeros(len(O.bodies)),vFluctZ = np.zeros(len(O.bodies)),ids = idApplyForce, label = 'hydroEngine'),
	#To plot the wall normal position of the spheres with time
	PyRunner(command = 'plotPos()', virtPeriod = 0.01, label = 'plot'),
	# Integrate the equation and calculate the new position/velocities...
	NewtonIntegrator(gravity=gravityVector, label='newtonIntegr')
	]
#save the initial configuration to be able to recharge the simulation starting configuration easily
O.saveTmp()
#Time step
O.dt = 5e-7 #Low no purpose, in order to observe the sedimentation

#Plot the wall normal position of the spheres with time
def plotPos():
	plot.addData(z1 = O.bodies[2].state.pos[2]/fluidHeight,z2 = O.bodies[3].state.pos[2]/fluidHeight,z3 = O.bodies[4].state.pos[2]/fluidHeight, time = O.time)
	if O.time>endTime:
		print('\nEnd of the simulation, {0}s simulated as asked!\n'.format(endTime))
		O.pause()

plot.plots={'time':('z1','z2','z3')}
plot.plot()
