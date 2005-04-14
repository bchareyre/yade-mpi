/***************************************************************************
 *   Copyright (C) 2005 by Janek Kozicki                                   *
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

#include "ActionParameterInitializer.hpp"
#include "ComplexBody.hpp"
#include "ClassFactory.hpp"
#include "ActionParameter.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ActionParameterInitializer::ActionParameterInitializer() 
{
	actionParameterNames.clear();
}

ActionParameterInitializer::~ActionParameterInitializer() 
{
}

void ActionParameterInitializer::registerAttributes()
{
	REGISTER_ATTRIBUTE(actionParameterNames);
}

void ActionParameterInitializer::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	
	vector<shared_ptr<ActionParameter> > actionParameters;
	actionParameters.clear();
	
	for(unsigned int i = 0 ; i < actionParameterNames.size() ; ++i )
		actionParameters.push_back(
			dynamic_pointer_cast<ActionParameter>
				(ClassFactory::instance().createShared(actionParameterNames[i]))
		);
	
	ncb->actionParameters->prepare(actionParameters);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

