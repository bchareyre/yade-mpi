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

#include "ApplyActionDispatcher.hpp"
#include "ComplexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ApplyActionDispatcher::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<applyActionFunctors.size();i++)
			actionDispatcher.add(applyActionFunctors[i][0],applyActionFunctors[i][1],applyActionFunctors[i][2]);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ApplyActionDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(applyActionFunctors);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ApplyActionDispatcher::addApplyActionFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	applyActionFunctors.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ApplyActionDispatcher::action(Body* body)
{

// NEW VERSION - 107 seconds - faster
//
// there are a lot of places where we can do improvement like this. if we do code change
// like here - in every possible places - yade can gain a lot of speed.
//
// basically I've discovered that every temporary variable of type shared_ptr<> costs 3 seconds,
// so we should avoid creating them and instead pass them around by references - whenever possible
//


	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>* bodies_ptr = &(ncb->bodies);
	shared_ptr<Action>* action_ptr;

	int id;
	for( ncb->actions->gotoFirst() ; ncb->actions->notAtEnd() ; ncb->actions->gotoNext())
	{
		action_ptr = &(ncb->actions->getCurrent(id));
		actionDispatcher( *action_ptr , (*(*bodies_ptr))[id]->physicalParameters);
// FIXME - this line would work if action was holding body's id. and it is possible that it will be even faster
//		actionDispatcher( ncb->actions->getCurrent(id) , (*(*bodies_ptr))[id]);
	}
		
		
/* OLD VERSION - 111 seconds

	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Action> action;

	for( ncb->actions->gotoFirst() ; ncb->actions->notAtEnd() ; ncb->actions->gotoNext())
	{
		int id;
		action = ncb->actions->getCurrent(id);
		
		shared_ptr<Body> b = (*bodies)[id];

		actionDispatcher( action, b);
	}


*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
