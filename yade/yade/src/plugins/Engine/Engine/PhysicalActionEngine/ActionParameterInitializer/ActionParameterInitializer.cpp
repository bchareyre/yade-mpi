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

#include "PhysicalActionContainerInitializer.hpp"
#include "MetaBody.hpp"
#include "ClassFactory.hpp"
#include "PhysicalAction.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionContainerInitializer::PhysicalActionContainerInitializer() 
{
	actionParameterNames.clear();
}

PhysicalActionContainerInitializer::~PhysicalActionContainerInitializer() 
{
}

void PhysicalActionContainerInitializer::registerAttributes()
{
	REGISTER_ATTRIBUTE(actionParameterNames);
}

void PhysicalActionContainerInitializer::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	
	vector<shared_ptr<PhysicalAction> > actionParameters;
	actionParameters.clear();
	
	for(unsigned int i = 0 ; i < actionParameterNames.size() ; ++i )
		actionParameters.push_back(
			dynamic_pointer_cast<PhysicalAction>
				(ClassFactory::instance().createShared(actionParameterNames[i]))
		);
	
	ncb->actionParameters->prepare(actionParameters);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

