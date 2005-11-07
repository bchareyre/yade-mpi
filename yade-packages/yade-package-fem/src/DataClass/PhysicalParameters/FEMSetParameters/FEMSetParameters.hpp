/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FEM_SET_PARAMETERS_HPP
#define FEM_SET_PARAMETERS_HPP 

#include <yade/yade-core/PhysicalParameters.hpp>

class FEMSetParameters : public PhysicalParameters
{
	public :
		int	 nodeGroupMask
			,tetrahedronGroupMask;

/// Constructor/Destructor
		FEMSetParameters();
		virtual ~FEMSetParameters();

/// Serializable
	REGISTER_CLASS_NAME(FEMSetParameters);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);

/// Indexable
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_INDEX(FEMSetParameters,PhysicalParameters);

};

REGISTER_SERIALIZABLE(FEMSetParameters,false);

#endif // LATTICEBEAMPARAMETERS_HPP

