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

#include "ActionApplyDispatcher.hpp"
#include "ComplexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionApplyDispatcher::postProcessAttributes(bool deserializing)
{
	postProcessDispatcher2D(deserializing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionApplyDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionApplyDispatcher::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	int id;
	for( ncb->actions->gotoFirst() ; ncb->actions->notAtEnd() ; ncb->actions->gotoNext())
	{
		shared_ptr<Action>& action = ncb->actions->getCurrent(id);
// FIXME - it would be better if action was holding body's id. and it is possible that it will be even faster
		operator()( action , (*bodies)[id]->physicalParameters);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
