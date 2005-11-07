/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FEM_NODE_PARAMETERS_HPP 
#define FEM_NODE_PARAMETERS_HPP 

#include <yade/yade-package-common/ParticleParameters.hpp>

class FEMNodeData : public ParticleParameters
{
/// Attributes 
	
	public :
		Vector3r initialPosition; // maybe this should be stored in FEMTetrahedron? (but then this data will be stored multiple times, since each tetrahedron will copy the same coordinades for each of its nodes...)
	
/// Methods 
		FEMNodeData();
		virtual ~FEMNodeData();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(FEMNodeData);
	REGISTER_BASE_CLASS_NAME(ParticleParameters);
	
/// Indexable
	REGISTER_CLASS_INDEX(FEMNodeData,ParticleParameters);

};

REGISTER_SERIALIZABLE(FEMNodeData,false);

#endif // FEM_NODE_PARAMETERS_HPP

