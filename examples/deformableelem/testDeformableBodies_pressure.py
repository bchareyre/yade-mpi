#!/usr/bin/python
# -*- coding: utf-8 -*-
# Created by Burak ER

#****** This is the test application of Longitudinal Wave propagation on Deformable Elements

#****** The application is consisted of a free-fixed 1 meter rod having a miliseconds sinusoidal pressure applied on the one side.

#****** The propagation of the wave is investigated.
from yade.deformableelementsutils import *;

O=Omega()

## Create deformable elements internal materials and interaction element material

mat=LinIsoRayleighDampElastMat(label='aluminiummaterial');# deformable body material

intermat=LinCohesiveStiffPropDampElastMat(label='intermat') # deformable bodies interaction material

intermat.youngmodulus=72800e6; #interaction material stiffness

mat.density=2789; # deformable body density

mat.youngmodulus=72800e7# deformable body youngs modulus

O.materials.append(intermat)# add interaction material to list

O.materials.append(mat)# add deformable body material to list

## Generate and add deformable elements into scene using GMSH2 mesh: 

# arg1 : mesh file
# arg2 : deformable element type
# arg3 : deformable element material
# arg4 : interaction element type
# arg5 : interaction element material
modelmesh=tetrahedral_mesh_generator('model.msh',Lin4NodeTetra,'aluminiummaterial',Lin4NodeTetra_Lin4NodeTetra_InteractionElement,'intermat')

## Define the body boundary force-> it is pressure applied on the one end of the rod

# x position limits of bodies that are subject to the force
force_tail_body_lowerlimit=-1.01;

force_tail_body_upperlimit=-0.9999;

# Pressure Properties

#pressure=Vector3(1.5e6,0,0);

pressure=Vector3(1.5e8,0,0);

period=(3e-4)

omega=2*pi/period;

applicationperiod=period/2;

# Displacement Properties(Displacement Application)

boundary_displacement_vector=Vector3(0.3,0,0);

## Define the fixed boundary

# x position limits of bodies that are subject to the fixed boundary

fixed_tail_body_lowerlimit=0.99;

fixed_tail_body_upperlimit=1.01;


def getinitialpos():
	positions=[];
	for body in O.bodies:
		positions.append(body.state.pos)
	return positions


def getboundarybodies(mesh):
    bdy = [];
    listofbodies = dict();
    listofboundarybodies = dict();
    for body in mesh:
        if (len(body) > 1):
            listofbodies[body[0].id] = [];
            for bdyy in body[1]:
                if (bdyy.shape.dispIndex == 12):
                    if (bdyy.state.pos[0] < fixed_tail_body_upperlimit):
                        if (bdyy.state.pos[0] > fixed_tail_body_lowerlimit):
                            listofbodies[body[0].id].append(bdyy);  #add node to boundary bodies of body
                            if (listofbodies[body[0].id].index(
                                    bdyy) == 2):  #if this body has three nodes of it on the boundary then add it to the traction force list.
                                listofboundarybodies[body[0].id] = listofbodies[body[0].id];
    return listofboundarybodies;


def getforcebodies(mesh):
    bdy = [];
    listofbodies = dict();
    listofboundarybodies = dict();
    for body in mesh:
        if(len(body) > 1):
            listofbodies[body[0].id] = [];
            for bdyy in body[1]:
                if (bdyy.shape.dispIndex == 12):
                    if (bdyy.state.pos[0] < force_tail_body_upperlimit):
                        if (bdyy.state.pos[0] > force_tail_body_lowerlimit):
                            listofbodies[body[0].id].append(bdyy);  #add node to boundary bodies of body
                            if (listofbodies[body[0].id].index(
                                    bdyy) == 2):  #if this body has three nodes of it on the boundary then add it to the traction force list.
                                listofboundarybodies[body[0].id] = listofbodies[body[0].id];


    return listofboundarybodies;

initialpositions=getinitialpos();

forcebodies=getforcebodies(modelmesh);

boundarybodies=getboundarybodies(modelmesh);

def displaceelements():
	for deformablebody in forcebodies:
		for node in forcebodies[deformablebody]:
			if(O.time<applicationperiod):
				O.bodies[node.id].state.pos=O.bodies[node.id].state.pos+boundary_displacement_vector*sin(omega*O.time);

## definition of functions
def fixboundaryelements():
	for deformablebody in boundarybodies:
		for node in boundarybodies[deformablebody]:
			O.forces.addF(node.id,-O.forces.f(node.id))
			O.forces.addT(node.id,-O.forces.m(node.id))

def applyforcetoelements():

	for deformablebody in forcebodies:
		pos0=O.bodies[forcebodies[deformablebody][0].id].state.pos;
		pos1=O.bodies[forcebodies[deformablebody][1].id].state.pos;
		pos2=O.bodies[forcebodies[deformablebody][2].id].state.pos;
		area=((pos1-pos0).cross(pos2-pos0)).norm();
		
		T=area*pressure;

		for node in forcebodies[deformablebody]:
			if(O.time<applicationperiod):
				O.forces.addF(node.id,(T/3)*sin(omega*O.time));
#		fixboundaryelements();

def addplot():
	forcenode=forcebodies[forcebodies.keys()[0]][0].id;
	boundarynode=boundarybodies[boundarybodies.keys()[0]][0].id;
	plot.addData(force=O.forces.f(forcenode)[0],pos=(O.bodies[forcenode].state.pos[0]-initialpositions[forcenode][0]),vel=O.bodies[forcenode].state.vel[0],postail=(O.bodies[boundarynode].state.pos[0]-initialpositions[boundarynode][0]),t=O.time,time=O.time,tm=O.time,tt=O.time)

		


## Adaptive integration engine initialization: we have given the engines in ordered, they are runned sequentally for every substep of the integrator

integratoreng=RungeKuttaCashKarp54Integrator([	## Resets forces and momenta the act on bodies
						ForceResetter(),	
						## Apply internal force to the deformable elements and internal force of the interaction element	 
						FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
						PyRunner(virtPeriod=1e-99,command='applyforcetoelements()'),

])
# Integration tolerances of the RungeKuttaCashKarp54Integrator

integratoreng.rel_err=1e-3;

integratoreng.abs_err=1e-3;

# We use only the integrator engine

# Time step determines the exiting period of the integrator since the integrator performs one step from current_time to current_time+dt;

O.dt=1e-3;

O.engines=[
#		integratoreng,
	    ForceResetter(),
#	    ## Apply internal force to the deformable elements and internal force of the interaction element
	    FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
	    PyRunner(iterPeriod=1,command='applyforcetoelements()'),
	    NewtonIntegrator(damping=0,gravity=[0,0,0]),
#	    ## Plotting data: adds plots after one step of the integrator engine
	    PyRunner(iterPeriod=1,command='addplot()')
	  ]

from yade import plot

plot.plots={'t':'vel','time':'pos','tm':'force','tt':'postail'}
plot.plot(subPlots=True)

try:
	from yade import qt
	qt.View()
	qt.Controller()
except ImportError: pass

