/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_BEAM_PARAMETERS_HPP
#define LATTICE_BEAM_PARAMETERS_HPP 

#include <yade/yade-package-common/RigidBodyParameters.hpp>

class LatticeBeamParameters : public RigidBodyParameters
{
	public :
		unsigned int 	id1,id2;
		Real  		initialLength,length;
		Vector3r 	direction;
	
		LatticeBeamParameters();
		virtual ~LatticeBeamParameters();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(LatticeBeamParameters);
	REGISTER_BASE_CLASS_NAME(RigidBodyParameters);
	
/// Indexable
	REGISTER_CLASS_INDEX(LatticeBeamParameters,RigidBodyParameters);

};

REGISTER_SERIALIZABLE(LatticeBeamParameters,false);

#endif // LATTICE_BEAM_PARAMETERS_HPP 

