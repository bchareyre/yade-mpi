/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef GEOMETRICAL_MODEL_DISPATCHER_HPP
#define GEOMETRICAL_MODEL_DISPATCHER_HPP

#include "GeometricalModelEngineUnit.hpp"
#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/Body.hpp>

class GeometricalModelMetaEngine :	public MetaDispatchingEngine2D
					<	
						PhysicalParameters,
						GeometricalModel,
						GeometricalModelEngineUnit,
						void ,
						TYPELIST_3(	  const shared_ptr<PhysicalParameters>&
								, shared_ptr<GeometricalModel>&
								, const Body*
				  			  )
					>
{
	public :
		virtual void action(Body* b);

	REGISTER_CLASS_NAME(GeometricalModelMetaEngine);
	REGISTER_BASE_CLASS_NAME(MetaDispatchingEngine2D);
};

REGISTER_SERIALIZABLE(GeometricalModelMetaEngine,false);

#endif // GEOMETRICAL_MODEL_DISPATCHER_HPP

