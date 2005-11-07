/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_SET_PARAMETERS_HPP
#define LATTICE_SET_PARAMETERS_HPP 


#include <yade/yade-core/PhysicalParameters.hpp>


class LatticeSetParameters : public PhysicalParameters
{
	public :
		int beamGroupMask;
		LatticeSetParameters();
		virtual ~LatticeSetParameters();

/// Serializable
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(LatticeSetParameters);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);

/// Indexable
	REGISTER_CLASS_INDEX(LatticeSetParameters,PhysicalParameters);

};

REGISTER_SERIALIZABLE(LatticeSetParameters,false);

#endif // LATTICE_SET_PARAMETERS_HPP 

