#!/usr/bin/python
# -*- coding: utf-8 -*-
# Created by Burak ER
from yade.deformableelementsutils import *;

O=Omega()

pressure=70e7;# N/m^2
wx=1;#m
wy=1;#m
wz=1;#m
## Create deformable elements internal materials and interaction element material

mat=LinIsoRayleighDampElastMat(label='aluminiummaterial');

intermat=LinCohesiveStiffPropDampElastMat(label='intermat')

intermat.youngmodulus=70e11;

mat.density=2700;

mat.beta=0.05;

mat.alpha=0.05;

mat.youngmodulus=70e9;

O.materials.append(intermat)

O.materials.append(mat)




#create points of a cube with number of 8 points
p0=Vector3([0, 0 ,0]);
p1=Vector3([0, wy, 0]);
p2=Vector3([wx ,0 ,0]);
p3=Vector3([0, 0 ,wz]);
p4=Vector3([0, wy, wz]);
p5=Vector3([wx, wy, 0]);
p6=Vector3([wx, 0 ,wz]);
p7=Vector3([wx, wy ,wz]);

nodesofelement1=[p0,p1,p2,p3];
nodesofelement2=[p3,p4,p1,p7];
nodesofelement3=[p2,p5,p1,p7];
nodesofelement4=[p7,p2,p3,p6];
nodesofelement5=[p1,p2,p3,p7];





noderadius=0.1*(0.75*tetrahedronvolume(nodesofelement1))**0.33333333;

[elbody1, nodebodies1]=tetrahedral_element(mat,nodesofelement1,Lin4NodeTetra,radius=noderadius)
[elbody2, nodebodies2]=tetrahedral_element(mat,nodesofelement2,Lin4NodeTetra,radius=noderadius)
[elbody3, nodebodies3]=tetrahedral_element(mat,nodesofelement3,Lin4NodeTetra,radius=noderadius)
[elbody4, nodebodies4]=tetrahedral_element(mat,nodesofelement4,Lin4NodeTetra,radius=noderadius)
[elbody5, nodebodies5]=tetrahedral_element(mat,nodesofelement5,Lin4NodeTetra,radius=noderadius)



interfaceelementpairs=[];
#interface of 1-5
interfaceelementpairs.append([nodebodies1[1], nodebodies5[0]]);
interfaceelementpairs.append([nodebodies1[2], nodebodies5[1]]);
interfaceelementpairs.append([nodebodies1[3], nodebodies5[2]]);

[elbodyinter, nodebodiesinter]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);

#interface of 2-5
interfaceelementpairs=[];
interfaceelementpairs.append([nodebodies2[0], nodebodies5[2]]);
interfaceelementpairs.append([nodebodies2[2], nodebodies5[0]]);
interfaceelementpairs.append([nodebodies2[3], nodebodies5[3]]);

[elbodyinter, nodebodiesinter]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);

#interface of 3-5
interfaceelementpairs=[];
interfaceelementpairs.append([nodebodies3[0], nodebodies5[1]]);
interfaceelementpairs.append([nodebodies3[2], nodebodies5[0]]);
interfaceelementpairs.append([nodebodies3[3], nodebodies5[3]]);

[elbodyinter, nodebodiesinter]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);

#interface of 3-5
interfaceelementpairs=[];
interfaceelementpairs.append([nodebodies3[0], nodebodies5[1]]);
interfaceelementpairs.append([nodebodies3[2], nodebodies5[0]]);
interfaceelementpairs.append([nodebodies3[3], nodebodies5[3]]);
[elbodyinter, nodebodiesinter]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);

#interface of 4-5
interfaceelementpairs=[];
interfaceelementpairs.append([nodebodies4[0], nodebodies5[3]]);
interfaceelementpairs.append([nodebodies4[1], nodebodies5[1]]);
interfaceelementpairs.append([nodebodies4[2], nodebodies5[2]]);
[elbodyinter, nodebodiesinter]=interaction_element(intermat,interfaceelementpairs,Lin4NodeTetra_Lin4NodeTetra_InteractionElement);


# Time step determines the exiting period of the integrator since the integrator performs one step from current_time to current_time+dt; using many substeps for any value of dt; then stops. 
initpos0=O.bodies[nodebodies1[0].id].state.pos;



def applyforcetoelements():
	
		pos0=O.bodies[nodebodies3[0].id].state.pos;
		pos1=O.bodies[nodebodies3[1].id].state.pos;
		pos2=O.bodies[nodebodies3[2].id].state.pos;
		area1=0.5*((pos1-pos0).cross(pos2-pos0)).norm();


		pos0=O.bodies[nodebodies1[0].id].state.pos;
		pos1=O.bodies[nodebodies1[1].id].state.pos;
		pos2=O.bodies[nodebodies1[2].id].state.pos;
		area2=0.5*((pos1-pos0).cross(pos2-pos0)).norm();


		Traction=area1*pressure;

		O.forces.addF(nodebodies3[0].id,Vector3(0,0,-(0.3*Traction)));
		O.forces.addF(nodebodies3[1].id,Vector3(0,0,-(0.3*Traction)));
		O.forces.addF(nodebodies3[2].id,Vector3(0,0,-(0.3*Traction)));


		
		Traction=area2*pressure;

		O.forces.addF(nodebodies1[0].id,Vector3(0,0,-(0.3*Traction)));
		O.forces.addF(nodebodies1[1].id,Vector3(0,0,-(0.3*Traction)));
		O.forces.addF(nodebodies1[2].id,Vector3(0,0,-(0.3*Traction)));

def fixboundryelements():
	
	nodebodies1[3].state.blockedDOFs='xyzXYZ'
	nodebodies2[0].state.blockedDOFs='xyzXYZ'
	nodebodies2[1].state.blockedDOFs='xyzXYZ'
	nodebodies2[3].state.blockedDOFs='xyzXYZ'
	nodebodies3[3].state.blockedDOFs='xyzXYZ'
	nodebodies4[0].state.blockedDOFs='xyzXYZ'
	nodebodies4[2].state.blockedDOFs='xyzXYZ'
	nodebodies4[3].state.blockedDOFs='xyzXYZ'
	nodebodies5[2].state.blockedDOFs='xyzXYZ'
	nodebodies5[3].state.blockedDOFs='xyzXYZ'


fixboundryelements();


def addplot():
	 vecpos=(O.bodies[nodebodies1[0].id].state.pos-initpos0)
	 plot.addData(displacement=abs(vecpos[2]),time=O.time)





 ##integratoreng=RungeKuttaCashKarp54Integrator([
 ##	    ForceResetter(),

	    ## Apply internal force to the deformable elements and internal force of the interaction element	 
 ##	    FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),

 ##	    PyRunner(virtPeriod=1e-99,command='applyforcetoelements()'),
	  

 ##])

 ##O.engines=[integratoreng,
 ##  PyRunner(iterPeriod=1,command='addplot()')
 ##]

 ##integratoreng.rel_err=1e-3;

 ##integratoreng.abs_err=1e-3;
O.engines=[ForceResetter(),
 	    PyRunner(virtPeriod=1e-99,command='applyforcetoelements()'), 
	    ## Apply internal force to the deformable elements and internal force of the interaction element	

 	    FEInternalForceEngine([If2_Lin4NodeTetra_LinIsoRayleighDampElast(),If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat()]),
	    NewtonIntegrator(damping=0,gravity=[0,0,0]),
 	    PyRunner(virtPeriod=1e-99,command='addplot()'), 

];




O.dt=1e-7;

from yade import plot

plot.plots={'time':'displacement'}

plot.plot(subPlots=False)

try:
	from yade import qt
	qt.View()
	qt.Controller()
except ImportError: pass

