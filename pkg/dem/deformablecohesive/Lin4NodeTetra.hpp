/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <core/Body.hpp>
#include <core/PartialEngine.hpp>
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <lib/base/Logging.hpp>
#include <lib/base/Math.hpp>


class NewtonIntegrator;

namespace yade{

#define VECTOR4_TEMPLATE(Scalar) Eigen::Matrix<Scalar,4,1>
typedef VECTOR4_TEMPLATE(Real) Vector4r;

class Lin4NodeTetra: public DeformableElement {
	public:

		friend class If2_Lin4NodeTetra_LinIsoRayleighDampElast;
		shared_ptr<Matrix> massMatrixInvProductstiffnessMatrix;
		Matrix calculateStiffness(Real, Real ,Vector3r,Vector3r,Vector3r,Vector3r);
		Matrix calculateMassMatrix(Real, Real);
		virtual ~Lin4NodeTetra();
		void initialize(void);
		Real getVolume(void){


			   NodeMap::iterator i0(localmap.begin());
			   NodeMap::iterator i1(i0);
			   NodeMap::iterator i2(i0);
			   NodeMap::iterator i3(i0);

			   std::advance(i1,1);
			   std::advance(i2,2);
			   std::advance(i3,3);

			   Matrix J(4,4);
			   Vector3r pos0=Vector3r(0,0,0);
			   Vector3r pos1=i1->second.position-i0->second.position;
			   Vector3r pos2=i2->second.position-i0->second.position;
			   Vector3r pos3=i3->second.position-i0->second.position;

         /*
			   Vector3r pos01=-pos1;
			   Vector3r pos02=-pos2;
			   Vector3r pos03=-pos3;
			   Vector3r pos12= pos1-pos2;
			   Vector3r pos13= pos1-pos3;
			   Vector3r pos23= pos2-pos3;
         */


	 			  J.col(0)<<1,pos0;
	 			  J.col(1)<<1,pos1;
	 			  J.col(2)<<1,pos2;
	 			  J.col(3)<<1,pos3;

          /*
	   		   Real x12=pos01(0), x13=pos02(0), x14=pos03(0), x23= pos12(0), x24=pos13(0), x34= pos23(0);

	   		   Real x21=-x12, x31=-x13, x41=-x14, x32=-x23, x42=-x24, x43=-x34;

	  		   Real y12=pos01(1),  y13=pos02(1),  y14=pos03(1),  y23= pos12(1),  y24=pos13(1),  y34= pos23(1);

	  		   Real y21=-y12,  y31=-y13,  y41=-y14,  y32=-y23,  y42=-y24,  y43=-y34;

	   		   Real z12=pos01(2),  z13=pos02(2),  z14=pos03(2),  z23= pos12(2),  z24=pos13(2),  z34= pos23(2);

	  		   Real z21=-z12,  z31=-z13,  z41=-z14,  z32=-z23,  z42=-z24,  z43=-z34;
          */
	  		   Real V=fabs(((0.166666667)*J.determinant()));

			   return V;
		}
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(Lin4NodeTetra,DeformableElement,"Tetrahedral Deformable Element Composed of Nodes",
		,
		,
		createIndex(); /*ctor*/
		initialize();
		,
		/*py*/

	);
		DECLARE_LOGGER;

		REGISTER_CLASS_INDEX(Lin4NodeTetra,DeformableElement);

};
}
// necessary
using namespace yade;
REGISTER_SERIALIZABLE(Lin4NodeTetra);
