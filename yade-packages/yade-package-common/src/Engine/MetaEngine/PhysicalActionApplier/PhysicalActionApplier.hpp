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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHYSICALACTIONAPPLIER_HPP__
#define __PHYSICALACTIONAPPLIER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalActionApplierUnit.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/PhysicalAction.hpp>


class Body;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class PhysicalActionApplier :	public MetaDispatchingEngine2D
				<
					PhysicalAction ,					// base classe for dispatch
					PhysicalParameters,					// base classe for dispatch
					PhysicalActionApplierUnit,				// class that provides multivirtual call
					void,							// return type
					TYPELIST_3(	  const shared_ptr<PhysicalAction>&	// function arguments
							, const shared_ptr<PhysicalParameters>& 
							, const Body *
				    		  )
				>
{
	public 		: virtual void action(Body* body);
	//public		: virtual void registerAttributes();
	//protected	: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(PhysicalActionApplier);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(PhysicalActionApplier,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PHYSICALACTIONAPPLIER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

