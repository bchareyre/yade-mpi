/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_NODE_PARAMETERS_HPP
#define LATTICE_NODE_PARAMETERS_HPP 

#include<yade/core/PhysicalParameters.hpp>
#include <vector>

class LatticeNodeParameters : public PhysicalParameters 
{
	public : 
		Real 		 countIncremental
				,countStiffness;
		Vector3r 	 displacementIncremental
				,displacementAlignmental;
	
		LatticeNodeParameters();
		virtual ~LatticeNodeParameters();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(LatticeNodeParameters);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);
	
/// Indexable
	REGISTER_CLASS_INDEX(LatticeNodeParameters,PhysicalParameters);

};

REGISTER_SERIALIZABLE(LatticeNodeParameters,false);

#endif // LATTICE_NODE_PARAMETERS_HPP 


