/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP
#define GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP

#include <yade/yade-core/EngineUnit1D.hpp>
#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>

class GLDrawInteractionGeometryFunctor :	public EngineUnit1D
						<
		 					void ,
		 					TYPELIST_2(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&) 
						>
{
	public : virtual ~GLDrawInteractionGeometryFunctor() {};
	REGISTER_CLASS_NAME(GLDrawInteractionGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(GLDrawInteractionGeometryFunctor,false);

#endif //  GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP

