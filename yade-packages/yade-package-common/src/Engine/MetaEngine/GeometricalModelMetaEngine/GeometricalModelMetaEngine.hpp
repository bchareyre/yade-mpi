/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
#ifndef GEOMETRICAL_MODEL_DISPATCHER_HPP
#define GEOMETRICAL_MODEL_DISPATCHER_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GeometricalModelEngineUnit.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/Body.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

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
	public		: virtual void action(Body* b);
	//public		: virtual void registerAttributes();
	//public		: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(GeometricalModelMetaEngine);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(GeometricalModelMetaEngine,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GEOMETRICAL_MODEL_DISPATCHER_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
