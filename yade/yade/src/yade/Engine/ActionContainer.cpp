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

#include "ActionContainer.hpp"
#include "Action.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionContainer::ActionContainer() : Serializable()
{
	action.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionContainer::~ActionContainer()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ActionContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(action);
};

/*
void ActionContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		action.clear();
	}
	else
	{
		action.clear();
		for( this->gotoFirst() ; this->notAtEnd() ; this->gotoNext() )
			action.push_back(this->getCurrent());
	}
};

void ActionContainer::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		this->clear();
		vector<shared_ptr<Action> >::iterator it    = action.begin();
		vector<shared_ptr<Action> >::iterator itEnd = action.end();
		for( ; it != itEnd ; ++it)
			this->insert(*it);
		action.clear();
	}
	else
	{
		action.clear();
	}
};
*/
