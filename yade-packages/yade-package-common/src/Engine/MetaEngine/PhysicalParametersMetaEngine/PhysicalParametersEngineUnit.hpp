/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BODY_PHYSICAL_PARAMETERS_FUNCTOR_HPP
#define BODY_PHYSICAL_PARAMETERS_FUNCTOR_HPP 

#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/EngineUnit1D.hpp>

/*! \brief
	Abstract interface for all classes that want to do something depending on PhysicalParameters (works like adding external virtual function)

	Currently it is used for LeapFron integration and creating FEM

	\param const shared_ptr<PhysicalParameters>&	PhysicalParameters' index (Indexable) is used to dispatch this call
	\param Body*					Body's pointer
	
*/

class PhysicalParametersEngineUnit :    public EngineUnit1D
					<
		 				void ,
		 				TYPELIST_2(	  const shared_ptr<PhysicalParameters>&
								, Body*
			   				  )
					>
{	
	REGISTER_CLASS_NAME(PhysicalParametersEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(PhysicalParametersEngineUnit,false);

#endif // __GEOMETRICALMODELFACTORY_H__

