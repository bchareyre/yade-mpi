/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/GeometricalModelEngineUnit.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/PhysicalParameters.hpp>
#include<yade/core/GeometricalModel.hpp>
#include<yade/core/Body.hpp>

class GeometricalModelMetaEngine :	public Dispatcher2D
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
		virtual void action();

	REGISTER_CLASS_NAME(GeometricalModelMetaEngine);
	REGISTER_BASE_CLASS_NAME(Dispatcher2D);
};

REGISTER_SERIALIZABLE(GeometricalModelMetaEngine);


