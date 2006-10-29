/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STIFFNESSMATRIX_HPP
#define STIFFNESSMATRIX_HPP

#include <yade/yade-core/PhysicalAction.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Matrix3.hpp>

/**
@author Bruno Chareyre 
*/


/*! \brief Data class for storage of the global stiffness of the contacts around one body

	The minimum eignen-period in a collection of contacting bodies can be computed from the eigen values "ei" of the global stiffness matrix. An acceptable approximation of max(ei) may be obtained using the diagonal terms of the non-diagonalised matrix. This class contains two Vector3r where the components of the vector are the diagonal terms of the matrix Mk (3 components for translation + 3 components for rotation).
*/

class StiffnessMatrix : public PhysicalAction
{
	public :
		//full matrix, not required in the current simplified calculation of critical dt which uses only the diagonal terms
		//Matrix3r stiffness; //"old" style
		Vector3r stiffness; //optimized style
		StiffnessMatrix();
		virtual ~StiffnessMatrix();

//		virtual void add(const shared_ptr<PhysicalAction>& a); // FIXME - not used
//		virtual void sub(const shared_ptr<PhysicalAction>& a); // FIXME - not used

		virtual void reset();
		virtual shared_ptr<PhysicalAction> clone();
	
/// Serialization
	REGISTER_CLASS_NAME(StiffnessMatrix);
	REGISTER_BASE_CLASS_NAME(PhysicalAction);
	
/// Indexable
	REGISTER_CLASS_INDEX(StiffnessMatrix,PhysicalAction);
};

REGISTER_SERIALIZABLE(StiffnessMatrix,false);

#endif // STIFFNESSMATRIX_HPP
