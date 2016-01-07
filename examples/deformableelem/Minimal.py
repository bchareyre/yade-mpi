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

nodesofelement1=[[0, 0 ,0],[0, 1, 0],[1 ,0 ,0],[0, 0 ,1]]
nodesofelement2=[[0, 0 ,0],[0, 1, 0],[1,0 ,0],[0, 0 ,-1]]


noderadius=0.1*(0.75*tetrahedronvolume(nodesofelement1))**0.33333333;

[elbody, nodebodies1]=tetrahedral_element(mat,nodesofelement1,Lin4NodeTetra,radius=noderadius)
[elbody, nodebodies2]=tetrahedral_element(mat,nodesofelement2,Lin4NodeTetra,radius=noderadius)
nodebodies1[3].state.pos+=Vector3(0,0,0.01);


interfaceelementpairs=[];
interfaceelementpairs.append([nodebodies1[0], nodebodies2[0]]);
interfaceelementpairs.append([nodebodies1[1], nodebodies2[1]]);
interfaceelementpairs.append([nodebodies1[2], nodebodies2[2]]);

[elbody, nodebodies]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);



#mesh=tetrahedral_mesh_generator('model.msh',Lin4NodeTetra,'aluminiummaterial',Lin4NodeTetra_Lin4NodeTetra_InteractionElement,'intermat')

## Define the body boundary force

# x position limits of bodies that are subject to the force
force_tail_body_lowerlimit=-1.01;

force_tail_body_upperlimit=-0.9999;

# Angular frequency of the force
amplitude=100000;

period=(1e-3)

omega=2*pi/period;

applicationperiod=period/2;

## Define the fixed boundary

# x position limits of bodies that are subject to the fixed boundary

fixed_tail_body_lowerlimit=0.99;

fixed_tail_body_upperlimit=1.01;



# Time step determines the exiting period of the integrator since the integrator performs one step from current_time to current_time+dt; using many substeps for any value of dt; then stops. 

O.dt=1e-9;

O.engines=[
	    ForceResetter(),	
	    ## Apply internal force to the deformable elements and internal force of the interaction element	 
	    FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
	    NewtonIntegrator(damping=0,gravity=[0,0,0]),
	   #PyRunner(virtPeriod=1e-99,command='applyforcetoelements()'),
	   # ## Plotting data: adds plots after one step of the integrator engine
	   #PyRunner(virtPeriod=1e-99,command='addplot()')
	  ]

from yade import plot

plot.plots={'t':'vel','time':'pos','tm':'force'}
plot.plot(subPlots=True)

try:
	from yade import qt
	qt.View()
	qt.Controller()
except ImportError: pass

