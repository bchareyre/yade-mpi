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

#include "InteractionPhysicsDispatcher.hpp"
#include "ComplexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionPhysicsDispatcher::postProcessAttributes(bool deserializing)
{
	postProcessDispatcher2D(deserializing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionPhysicsDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionPhysicsDispatcher::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& initialInteractions = ncb->initialInteractions;
	for( initialInteractions->gotoFirstPotential() ; initialInteractions->notAtEndPotential() ; initialInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = initialInteractions->getCurrent();

		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		operator()( b1->physicalParameters , b2->physicalParameters , interaction );
	}

	shared_ptr<InteractionContainer>& runtimeInteractions = ncb->runtimeInteractions;
	for( runtimeInteractions->gotoFirstPotential() ; runtimeInteractions->notAtEndPotential() ; runtimeInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = runtimeInteractions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];

		if (interaction->isReal)
			operator()( b1->physicalParameters , b2->physicalParameters , interaction );
	}
}
