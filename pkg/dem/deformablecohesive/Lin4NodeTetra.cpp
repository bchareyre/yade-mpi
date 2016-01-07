/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <pkg/dem/deformablecohesive/Lin4NodeTetra.hpp>
#include <algorithm>
#include <core/Scene.hpp>
#include <core/BodyContainer.hpp>
#include <core/State.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/dem/deformablecohesive/Node.hpp>
#include <lib/base/Math.hpp>

YADE_PLUGIN((Lin4NodeTetra));
CREATE_LOGGER(Lin4NodeTetra);

Lin4NodeTetra::~Lin4NodeTetra(){

}
void Lin4NodeTetra::initialize(void){
	maxNodeCount=4;
}
Matrix Lin4NodeTetra::calculateMassMatrix(Real density,Real v)
{
	   Matrix mass(12,12);

	   mass<<2,0,0,1,0,0,1,0,0,1,0,0,
			 0,2,0,0,1,0,0,1,0,0,1,0,
			 0,0,2,0,0,1,0,0,1,0,0,1,
			 1,0,0,2,0,0,1,0,0,1,0,0,
			 0,1,0,0,2,0,0,1,0,0,1,0,
			 0,0,1,0,0,2,0,0,1,0,0,1,
			 1,0,0,1,0,0,2,0,0,1,0,0,
			 0,1,0,0,1,0,0,2,0,0,1,0,
			 0,0,1,0,0,1,0,0,2,0,0,1,
			 1,0,0,1,0,0,1,0,0,2,0,0,
			 0,1,0,0,1,0,0,1,0,0,2,0,
			 0,0,1,0,0,1,0,0,1,0,0,2;

	   mass=0.05*(density*v)*mass;
	   return mass;
}


Matrix Lin4NodeTetra::calculateStiffness(Real E, Real v,Vector3r pos0,Vector3r pos1,Vector3r pos2,Vector3r pos3)
{
		/*
		 * The calculation is relative, therefore, we do not need position of the first node.
		 * All other nodes are calculated with respect to first node which is at zero on local coordinates.
		 * The local coordinates are the coordinates that is fixed at node0 and parallel to global axes.
		 *
		 * */

	   Vector3r pos01=-pos1;
	   Vector3r pos02=-pos2;
	   Vector3r pos03=-pos3;
	   Vector3r pos12= pos1-pos2;
	   Vector3r pos13= pos1-pos3;
	   Vector3r pos23= pos2-pos3;

	   Matrix J(4,4);
	
	  

	   J.col(0)<<1,pos0;
	   J.col(1)<<1,pos1;
	   J.col(2)<<1,pos2;
	   J.col(3)<<1,pos3;


	   Real x12=pos01(0), x13=pos02(0), x14=pos03(0), x23= pos12(0), x24=pos13(0), x34= pos23(0);

	   Real x21=-x12, x31=-x13, x41=-x14, x32=-x23, x42=-x24, x43=-x34;

	   Real y12=pos01(1),  y13=pos02(1),  y14=pos03(1),  y23= pos12(1),  y24=pos13(1),  y34= pos23(1);

	   Real y21=-y12,  y31=-y13,  y41=-y14,  y32=-y23,  y42=-y24,  y43=-y34;

	   Real z12=pos01(2),  z13=pos02(2),  z14=pos03(2),  z23= pos12(2),  z24=pos13(2),  z34= pos23(2);

	   Real z21=-z12,  z31=-z13,  z41=-z14,  z32=-z23,  z42=-z24,  z43=-z34;

	   Real V=fabs(((0.166666667)*J.determinant()));
		


	   Real a1=y42*z32-y32*z42, b1=x32*z42-x42*z32, c1=x42*y32-x32*y42;
	   Real a2=y31*z43-y34*z13, b2=x43*z31-x13*z34, c2=x31*y43-x34*y13;
	   Real a3=y24*z14-y14*z24, b3=x14*z24-x24*z14, c3=x24*y14-x14*y24;
	   Real a4=y13*z21-y12*z31, b4=x21*z13-x31*z12, c4=x13*y21-x12*y31;

	  //std::cout<<"Volume of the element is "<<V<<"\n";
 
	   Matrix B(6,12);
	   //Construction of B matrix
	   B<<a1,0,0,a2,0,0,a3,0,0,a4,0,0,
	      0,b1,0,0,b2,0,0,b3,0,0,b4,0,
	      0,0,c1,0,0,c2,0,0,c3,0,0,c4,
	      b1,a1,0,b2,a2,0,b3,a3,0,b4,a4,0,
	      0,c1,b1,0,c2,b2,0,c3,b3,0,c4,b4,
	      c1,0,a1,c2,0,a2,c3,0,a3,c4,0,a4;
		//cout<<"B is"<<std::endl<<B<<std::endl;

	   B=(1/(6*V))*B;

	   //Construction of Elasticity matrix
	   Matrix Emtr(6,6);

	   Emtr<<1-v,v,v,0,0,0,
			v,1-v,v,0,0,0,
			v,v,1-v,0,0,0,
			0,0,0,0.5-v,0,0,
			0,0,0,0,0.5-v,0,
			0,0,0,0,0,0.5-v;

	   Emtr=E/((1+v)*(1-v))*Emtr;
	   Matrix stiffness=V*B.transpose()*Emtr*B;

	   return stiffness;

}



