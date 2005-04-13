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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ActionReset.hpp"
#include "ComplexBody.hpp"
#include "ActionForce.hpp"
#include "ActionMomentum.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionReset::ActionReset() : actionForce(new ActionForce) , actionMomentum(new ActionMomentum)
{
	first = true;
}

void ActionReset::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	
	if(first) // FIXME - this should be done somewhere else, or this is the right place ?
	{
		vector<shared_ptr<ActionParameter> > vvv; 
		vvv.clear();
		vvv.push_back(actionForce); // FIXME - should ask what Actions should be prepared !
		vvv.push_back(actionMomentum);
		ncb->actions->prepare(vvv);
		first = false;
	}
	
	ncb->actions->reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

