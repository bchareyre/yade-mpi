/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWGEOMETRICALMODELFUNCTOR_HPP
#define GLDRAWGEOMETRICALMODELFUNCTOR_HPP

#include <yade/yade-core/EngineUnit1D.hpp>
#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>


class GLDrawGeometricalModelFunctor : 	public EngineUnit1D
					<
		 				void ,
		 				TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool) 
					>
{
	public : 
		bool wire; // FIXME - right place ?
		virtual ~GLDrawGeometricalModelFunctor() {};
	
	REGISTER_CLASS_NAME(GLDrawGeometricalModelFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(GLDrawGeometricalModelFunctor,false);

#endif //  GLDRAWGEOMETRICALMODELFUNCTOR_HPP


