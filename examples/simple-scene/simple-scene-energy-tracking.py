#!/usr/bin/python
# -*- coding: utf-8 -*-

############################################
##### interesting parameters          #####
############################################
# Cundall non-viscous damping
# try zero damping and watch total energy...
damping = 0.2
# initial angular velocity
angVel = 3.0
# use two spheres?
two_spheres = False
# sphere rotating more?
rotate_in_two_directions = False

############################################
##### material                         #####
############################################

import matplotlib
matplotlib.use('TkAgg')
O.materials.append(CohFrictMat(
	young=3e8,
	poisson=0.3,
	frictionAngle=radians(30),
	density=2600,
	isCohesive=False,
	alphaKr=0.031,
	alphaKtw=0.031,
	momentRotationLaw=False,
	etaRoll=5.0,
	label='granular_material'))

############################################
##### calculation loop                 #####
############################################
law=Law2_ScGeom_CohFrictPhys_CohesionMoment(always_use_moment_law=False)
g=9.81

O.trackEnergy=True
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom6D(),Ig2_Box_Sphere_ScGeom6D()],	
		[Ip2_2xCohFrictMat_CohFrictPhys()],
		[law]
	),
	GravityEngine(gravity=(0,0,-g)),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=50),
	NewtonIntegrator(damping=damping,kinSplit=True),
	PyRunner(iterPeriod=20,command='myAddPlotData()')
]

from yade import utils
O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],material='granular_material'))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],material='granular_material'))
if(two_spheres):
	O.bodies.append(utils.sphere([0,0,4],1,color=[0,1,0],material='granular_material'))
O.dt=.002*utils.PWaveTimeStep()
O.bodies[1].state.angVel[1]=angVel
if(rotate_in_two_directions):
	O.bodies[1].state.angVel[2]=angVel

############################################
##### now the part pertaining to plots #####
############################################

from math import *
from yade import plot
## we will have 2 plots:
## 1. t as function of i (joke test function)
## 2. i as function of t on left y-axis ('|||' makes the separation) and z_sph, v_sph (as green circles connected with line) and z_sph_half again as function of t

plot.labels={'t':'time [s]', 
	'normal_Work':'Normal work: W=kx^2/2', 
	'shear_Work':'Shear work: W=kx^2/2',
	'E_kin_translation':'Translation energy: E_kin=m*V^2/2',
	'E_kin_rotation':'Rotation energy: E_kin=I*$\omega$^2/2',
	'E_pot':'Gravitational potential: E_pot=m*g*h',
	'total':'total',
	'total_plus_damp':'total + daping'}

plot.plots={'t':('normal_Work',
		'shear_Work',
		'E_kin_translation',
		'E_kin_rotation',
		#'E_kin_r','E_kin_tr','E_pot_',  ## those are from energy tracker
		'E_pot',
		'total',
		'total_plus_damp')}

## this function is called by plotDataCollector
## it should add data with the labels that we will plot
## if a datum is not specified (but exists), it will be NaN and will not be plotted
def myAddPlotData():
	sph=O.bodies[1]
	normal_Work	  = law.normElastEnergyForce()
	shear_Work	  = law.shearElastEnergyForce()
	E_kin_translation = 0
	E_kin_rotation    = 0
	E_pot		  = 0

	if(two_spheres):## for more bodies we better use the energy tracker, because it's tracking all bodies
		E_kin_translation = dict(O.energy.items())['kinTrans']
		E_kin_rotation    = dict(O.energy.items())['kinRot']
		E_pot		  = dict(O.energy.items())['gravWork'] 

	else: ## for one sphere we can just calculate, and it will be correct
		h=sph.state.pos[2]
		V=sph.state.vel.norm()
		w=sph.state.angVel.norm()
		m=sph.state.mass
		I=sph.state.inertia[0]
		E_kin_translation = m*V**2.0/2.0
		E_kin_rotation    = I*w**2.0/2.0
		E_pot		  = m*g*h

	total = normal_Work + shear_Work + E_kin_translation + E_kin_rotation + E_pot
	total_plus_damp	  = 0
	if(damping!=0):
		total_plus_damp	  = total + dict(O.energy.items())['nonviscDamp']
	else:	
		total_plus_damp	  = total
	plot.addData(
		t=O.time,
		normal_Work	  = normal_Work	 ,
		shear_Work	  = shear_Work	 ,
		E_kin_translation = E_kin_translation,
		E_kin_rotation    = E_kin_rotation   ,
		E_pot		  = E_pot		 ,
		total		  = total		 ,
		total_plus_damp	  = total_plus_damp	 ,
	)

print "Now calling plot.plot() to show the figures. The timestep is artificially low so that you can watch graphs being updated live."
plot.liveInterval=2
plot.plot(subPlots=True)
#from yade import qt
#qt.View()
O.run(int(2./O.dt));
#plot.saveGnuplot('/tmp/a')
## you can also access the data in plot.data['t'], etc, under the labels they were saved.

