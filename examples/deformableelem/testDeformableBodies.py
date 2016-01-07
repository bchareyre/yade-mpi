#!/usr/bin/python
# -*- coding: utf-8 -*-
# Created by Burak ER
from yade.deformableelementsutils import *;

O=Omega()

## Create deformable elements internal materials and interaction element material

mat=LinIsoRayleighDampElastMat(label='aluminiummaterial');

intermat=LinCohesiveStiffPropDampElastMat(label='intermat')

intermat.youngmodulus=70e11;

mat.density=2700;

mat.youngmodulus=70e9

O.materials.append(intermat)

O.materials.append(mat)

## Generate deformable element mesh from GMSH2 format

mesh=tetrahedral_mesh_generator('model.msh',Lin4NodeTetra,'aluminiummaterial',Lin4NodeTetra_Lin4NodeTetra_InteractionElement,'intermat')

## Define the body boundary force

# x position limits of bodies that are subject to the force
force_tail_body_lowerlimit=-1.01;

force_tail_body_upperlimit=-0.9999;

# Angular frequency of the force
amplitude=1000;

period=(1e-4)

omega=2*pi/period;

applicationperiod=period/2;

## Define the fixed boundary

# x position limits of bodies that are subject to the fixed boundary

fixed_tail_body_lowerlimit=0.99;

fixed_tail_body_upperlimit=1.01;


def getinitialpos():
	positions=[];
	for body in O.bodies:
		positions.append(body.state.pos)
	return positions

def getboundarybodies():
	bdy=[];
	for body in O.bodies:
			if(body.shape.dispIndex==12):
				if(body.state.pos[0]<fixed_tail_body_upperlimit):
					if(body.state.pos[0]>fixed_tail_body_lowerlimit):
						bdy.append(body.id)
	return bdy
						
def getforcebodies():
	bdy=[];
	for body in O.bodies:
			if(body.shape.dispIndex==12):
				if(body.state.pos[0]<force_tail_body_upperlimit):
					if(body.state.pos[0]>force_tail_body_lowerlimit):
						if(O.time<applicationperiod):
							bdy.append(body.id)	
	return bdy						

initialpositions=getinitialpos();

forcebodies=getforcebodies();

boundarybodies=getboundarybodies();


## definition of functions
def fixboundaryelements():
	for i in boundarybodies:
		O.forces.addF(i,-O.forces.f(i))
		O.forces.addT(i,-O.forces.m(i))

def applyforcetoelements():
	for i in forcebodies:
		if(O.time<applicationperiod):
#			print 'apply force @'+str(O.time)
			O.forces.addF(i,-1*O.forces.f(i));
	
			O.forces.addF(i,Vector3(sin(omega*O.time)*amplitude,0,0));
#			plot.addData(force=O.forces.f(i));

def addplot():
	 plot.addData(force=O.forces.f(forcebodies[1])[0],pos=(O.bodies[forcebodies[1]].state.pos[0]-initialpositions[forcebodies[1]][0]),vel=O.bodies[forcebodies[1]].state.vel[0],t=O.time,time=O.time,tm=O.time)
		
for i in forcebodies:
	O.bodies[i].state.pos+=Vector3(0,0,0.05);

## Adaptive integration engine initialization: we have given the engines in ordered, they are runned sequentally for every substep of the integrator

#integratoreng=RungeKuttaCashKarp54Integrator([	## Resets forces and momenta the act on bodies
#						ForceResetter(),	
#						## Apply internal force to the deformable elements and internal force of the interaction element	 
#						FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
#						PyRunner(virtPeriod=1e-99,command='applyforcetoelements()'),
#
#])
# Integration tolerances of the RungeKuttaCashKarp54Integrator
#~ 
#~ integratoreng.rel_err=1e-6;
#~ 
#~ integratoreng.abs_err=1e-6;

# We use only the integrator engine

# Time step determines the exiting period of the integrator since the integrator performs one step from current_time to current_time+dt; using many substeps for any value of dt; then stops. 

O.dt=1e-8;

O.engines=[
	    ForceResetter(),	
	    ## Apply internal force to the deformable elements and internal force of the interaction element	 
	    FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
	    PyRunner(iterPeriod=1,command='applyforcetoelements()'),
	    NewtonIntegrator(damping=0,gravity=[0,0,0]),
#	    ## Plotting data: adds plots after one step of the integrator engine
	    PyRunner(iterPeriod=1,command='addplot()')


];
#Tolerances can be set for the optimum accuracy


from yade import plot

plot.plots={'t':'vel','time':'pos','tm':'force'}
plot.plot(subPlots=True)

try:
	from yade import qt
	qt.View()
	qt.Controller()
except ImportError: pass

