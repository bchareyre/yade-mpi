/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
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

#ifndef __METADISPATCHINGENGINE2D_HPP__
#define __METADISPATCHINGENGINE2D_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//#include "MetaEngine.hpp"
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////
//#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// #define REGISTER_ENGINE_UNIT_TYPE(name)		\
// 	public : virtual string getEngineUnitType() { return name; };
// 
// template
// <
// 	class baseClass1,
// 	class baseClass2,
// 	class EngineUnit,
// 	class EngineUnitReturnType,
// 	class EngineUnitArguments
// >
// class MetaDispatchingEngine2D : public MetaEngine,
// 				public DynLibDispatcher
// 				<	  TYPELIST_2(baseClass1,baseClass2)	// base classes for dispatch
// 					, EngineUnit				// class that provides multivirtual call
// 					, EngineUnitReturnType			// return type
// 					, EngineUnitArguments			// argument of engine unit
// 				>
// {
// 	public : MetaDispatchingEngine2D() {};
// 	public : virtual ~MetaDispatchingEngine2D() {};
// 
// 	public		: virtual void registerAttributes();
// 	protected	: virtual void postProcessAttributes(bool deserializing);
// 
// 	public : virtual string getEngineUnitType() { throw; };
// 
// 	REGISTER_CLASS_NAME(MetaDispatchingEngine2D);
// 
// };
// 
// /////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////
// 
// REGISTER_SERIALIZABLE(MetaDispatchingEngine2D,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __METADISPATCHINGENGINE2D_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

