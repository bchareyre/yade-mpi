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

#ifndef __ACTIONDISPATCHER_HPP__
#define __ACTIONDISPATCHER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Actor.hpp"
#include "DynLibDispatcher.hpp"
#include "Action.hpp"
#include "ActionApplyFunctor.hpp"
#include "BodyPhysicalParameters.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class ActionApplyDispatcher : 
	  public Actor
	, public DynLibDispatcher
		<	TYPELIST_2( Action , BodyPhysicalParameters ) ,	// base classess for dispatch
			ActionApplyFunctor,				// class that provides multivirtual call
			void ,						// return type
			TYPELIST_2(	  const shared_ptr<Action>&
					, shared_ptr<BodyPhysicalParameters>& 
				)
		>
{
	public 		: virtual void action(Body* body);
	public		: virtual void registerAttributes();
	protected	: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(ActionApplyDispatcher);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(ActionApplyDispatcher,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ACTIONDISPATCHER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

