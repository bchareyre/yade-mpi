#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
This example shows how to generate a particle packing with given particle-size distribution curve (psd) and a given porosity.
This method works well for loose packings and low number of particles. Be careful, when particle sizes are low!

First step is to generate the particles with decreased radii (see radius denominator, rad_denom). In the next step the radius 
of all particles is expanded step-by-step by radius multipliers (see rad_mul). After each expansion high overlaps can occur.
Therefore the model has to calm down before the next expansion can be made (see calm() function).
"""

# USER INPUT :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

rho_p					= 2650	#density of particles
friction=0.5					#friction coefficient
angle=atan(friction)

initial_porosity		= 0.4	#set a target porosity

#SET GRAIN DISTRIBUTION:
#diameter at the borders of the classes from lowest to highest in [m]:
dia = [0.000063, 0.00009, 0.000125, 0.00018, 0.00025, 0.000355, 0.0005, 0.00071, 0.001, 0.002]
num_borders = len(dia)
	
#one-hundredth of volume-percent of classes out of sieve analysis:
phi = [0.0004, 0.0046, 0.0234, 0.1467, 0.4657, 0.2886, 0.0641, 0.0071, 0.0011]
num_classes = len(phi)

#SET DIMENSIONS OF THE MODEL (only cuboid possible in actual version):
x_cl = 0		#lower x coordinate in front
y_cl = 0		#lower y coordinate in front
z_cl = 0   		#lower z coordinate in front
x_cu = 0.002	#upper x coordinate in back
y_cu = 0.002	#upper y coordinate in back
z_cu = 0.002	#upper z coordinate in back
  
#SET RADIUS MULTIPLIER CONSTANTS (for blowing up balls during generation process):
rad_mul = [2, 1.8, 1.5, 1.2, 1.05]
num_rad_mul = len(rad_mul)

# END USER INPUT :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


# DEFINE MATERIALS AND ENGINES :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

id_FacetMat=O.materials.append(ViscElMat(kn=1e8,ks=1e8,cn=0.0,cs=0.0,frictionAngle=angle))
id_SphereMat=O.materials.append(ViscElMat(kn=1e6,ks=1e6,cn=0.0,cs=0.0,density=rho_p,frictionAngle=angle))

FacetMat=O.materials[id_FacetMat]
SphereMat=O.materials[id_SphereMat]

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=1,timestepSafetyCoefficient=0.8, defaultDt=4*utils.PWaveTimeStep()),
	NewtonIntegrator(damping=0.7)
]

# END DEFINE MATERIALS AND ENGINES :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


# GENERATION PROCESS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

V_c = (x_cu-x_cl)*(y_cu-y_cl)*(z_cu-z_cl)	#total volume of cuboid
V_part = V_c*(1-initial_porosity)			#volume of all particles

rad_denom = 1
for ii in range(0,num_rad_mul):
	rad_denom = rad_denom*rad_mul[ii]
	
#step1: calculation of volumes of spheres at borders
rad_borders = [1]*num_borders
Vg_borders = [1]*num_borders
for ii in range(0,num_borders):
	rad_borders[ii] = 0.5*dia[ii]			#get radii from diameters
	Vg_borders[ii] = 4*math.pi*(rad_borders[ii]*rad_borders[ii]*rad_borders[ii])/3	#volumes of spheres at borders

#step2: calculation of numbers of spheres of spheres in every class
Vd_classes = [1]*num_classes
rad_d = [1]*num_classes
V_classes = [1]*num_classes
num_spheres = [1]*num_classes
for ii in range(0,num_classes):
	Vd_classes[ii] 	= (Vg_borders[ii+1]+Vg_borders[ii])/2.0		#average volumes in every class
	rad_d[ii] 		= pow(3*Vd_classes[ii]/(4*math.pi),1.0/3.0)		#average radii in every class
	V_classes[ii] 	= phi[ii]*V_part							#particle volumes in every class
	#numbers of spheres in every class:
	num_spheres[ii] = int(round((3*V_classes[ii])/(4*math.pi*rad_d[ii]*rad_d[ii]*rad_d[ii])))

sum_spheres = -1
#step3: get random coordinates and radii and generate particles
for jj in range(0,num_classes):
	for ii in range(0,num_spheres[jj]):
		r_tmp = rad_borders[jj] + (rad_borders[jj+1] - rad_borders[jj])*random.random() #random radii
		x_tmp = r_tmp + x_cl + ((x_cu-x_cl) - 2*r_tmp)*random.random()	#random coordinates x, y, z
		y_tmp = r_tmp + y_cl + ((y_cu-y_cl) - 2*r_tmp)*random.random()
		z_tmp = r_tmp + z_cl + ((z_cu-z_cl) - 2*r_tmp)*random.random()
		r_tmp = r_tmp/rad_denom		#decrease radii for generation process
		sum_spheres=O.bodies.append(utils.sphere([x_tmp,y_tmp,z_tmp], material=SphereMat, radius=r_tmp, wire=False, highlight=False))#, color=(50,50,0)))

O.bodies.append(geom.facetBox(((x_cu-x_cl)/2.0,(y_cu-y_cl)/2.0,(z_cu-z_cl)/2.0),((x_cu-x_cl)/2.0,(y_cu-y_cl)/2.0,(z_cu-z_cl)/2.0),fixed=True,material=FacetMat))

# END GENERATION PROCESS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#DEFINITIONS:

def calm():
	for ii in range(0,sum_spheres+1):
		O.bodies[ii].state.vel		= Vector3(0,0,0)
		O.bodies[ii].state.angVel	= Vector3(0,0,0)

# RELAXATION PROCESS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

from yade import qt
v=qt.View()
raw_input('press ENTER to start RELAXATION')

O.engines=O.engines+[PyRunner(iterPeriod=10,command='calm()',label='calmLabel1')]
O.run(1000,True)

for ii in range(0,sum_spheres+1):
	O.bodies[ii].shape.radius=O.bodies[ii].shape.radius*rad_mul[0]
print ('radius of particles increased by factor %f, step 1 of 5' % rad_mul[0])
O.run(3000,True)

for ii in range(0,sum_spheres+1):
	O.bodies[ii].shape.radius=O.bodies[ii].shape.radius*rad_mul[1]
print ('radius of particles increased by factor %f, step 2 of 5' % rad_mul[1])
O.run(3000,True)
calmLabel1.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=20,command='calm()',label='calmLabel2')]
O.run(3000,True)

for ii in range(0,sum_spheres+1):
	O.bodies[ii].shape.radius=O.bodies[ii].shape.radius*rad_mul[2]
print ('radius of particles increased by factor %f, step 3 of 5' % rad_mul[2])
calmLabel2.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=10,command='calm()',label='calmLabel3')]
O.run(7000,True)
calmLabel3.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=20,command='calm()',label='calmLabel4')]
O.run(7000,True)

for ii in range(0,sum_spheres+1):
	O.bodies[ii].shape.radius=O.bodies[ii].shape.radius*rad_mul[3]
print ('radius of particles increased by factor %f, step 4 of 5' % rad_mul[3])
calmLabel4.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=10,command='calm()',label='calmLabel5')]
O.run(25000,True)
calmLabel5.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=20,command='calm()',label='calmLabel6')]
O.run(25000,True)

for ii in range(0,sum_spheres+1):
	O.bodies[ii].shape.radius=O.bodies[ii].shape.radius*rad_mul[4]
print ('radius of particles increased by factor %f, step 5 of 5' % rad_mul[4])
calmLabel6.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=10,command='calm()',label='calmLabel7')]
O.run(25000,True)
calmLabel7.dead=True
O.engines=O.engines+[PyRunner(iterPeriod=20,command='calm()',label='calmLabel8')]
O.run(25000,True)

# END RELAXATION PROCESS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
