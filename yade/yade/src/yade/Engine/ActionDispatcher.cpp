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

#include "ActionDispatcher.hpp"
#include "NonConnexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionDispatcher::ActionDispatcher() : Actor()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionDispatcher::~ActionDispatcher()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionDispatcher::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<actionFunctors.size();i++)
			actionDispatcher.add(actionFunctors[i][0],actionFunctors[i][1],actionFunctors[i][2]);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(actionFunctors);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionDispatcher::addActionFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	actionFunctors.push_back(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionDispatcher::action(Body* body)
{
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Action> action;

	for( ncb->actions->gotoFirst() ; ncb->actions->notAtEnd() ; ncb->actions->gotoNext())
	{
		int id;
		action = ncb->actions->getCurrent(id);
		
		shared_ptr<Body> b = (*bodies)[id];

		actionDispatcher( action, b);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
