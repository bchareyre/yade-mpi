/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FEM_TETRAHEDRON_PARAMETERS_HPP
#define FEM_TETRAHEDRON_PARAMETERS_HPP 

// FIXME - this file should be located in: /plugins/Interaction/InteractionPhysicsEngineUnit/FEMParameters/FEMTetrahedronStiffness

#include <yade/yade-package-common/PhysicalParametersEngineUnit.hpp>

class FEMTetrahedronStiffness : public PhysicalParametersEngineUnit
{

	private :
		int	 nodeGroupMask
			,tetrahedronGroupMask;

	public :
		virtual void go(	  const shared_ptr<PhysicalParameters>&
					, Body*);
	
	REGISTER_CLASS_NAME(FEMTetrahedronStiffness);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);

};

REGISTER_SERIALIZABLE(FEMTetrahedronStiffness,false);

#endif // FEM_TETRAHEDRON_PARAMETERS_HPP 

