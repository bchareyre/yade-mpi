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

#include "SDECTimeStepper.hpp"
#include "Interaction.hpp"
#include "ComplexBody.hpp"
#include "SDECParameters.hpp"
#include "Sphere.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECTimeStepper::SDECTimeStepper() : Actor()
{
	sdecGroup = 0;
	interval = 10;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECTimeStepper::~SDECTimeStepper()
{

}

bool SDECTimeStepper::isActivated()
{
	return Omega::instance().getCurrentIteration() % interval == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECTimeStepper::registerAttributes()
{
	REGISTER_ATTRIBUTE(sdecGroup);
	REGISTER_ATTRIBUTE(interval);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECTimeStepper::findTimeStepFromBody(const Body* body)
{
	SDECParameters * sdec	= dynamic_cast<SDECParameters*>(body->physicalParameters.get());
	Sphere* sphere 		= dynamic_cast<Sphere*>(body->geometricalModel.get());

	if(! (sphere && sdec) )
		return; // not possible to compute!

	Real rad3 = std::pow(sphere->radius,3); // radius to the power of 3, from sphere

	Real dt = 0.1*//min(
			  sqrt( sdec->mass    / sdec->kn      )
		//	, sqrt( sdec->inertia / sdec->ks*rad3 )
		//  )
		;

	newDt = std::min(dt,newDt);
	computedSomething = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECTimeStepper::findTimeStep(const shared_ptr<Interaction>& interaction, shared_ptr<BodyContainer>& bodies)
{
	unsigned int id1 = interaction->getId1();
	unsigned int id2 = interaction->getId2();
		
	if( (*bodies)[id1]->getGroup() != sdecGroup || (*bodies)[id2]->getGroup() != sdecGroup )
		return; // skip other groups

	Body* body1 = (*bodies)[id1].get();
	Body* body2 = (*bodies)[id2].get();

	findTimeStepFromBody(body1);
	findTimeStepFromBody(body2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECTimeStepper::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	shared_ptr<InteractionContainer>& permanentInteractions = ncb->permanentInteractions;
	shared_ptr<InteractionContainer>& interactions = ncb->interactions;

	newDt = Mathr::MAX_REAL;
	computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)

	for( permanentInteractions->gotoFirst() ; permanentInteractions->notAtEnd() ; permanentInteractions->gotoNext() )
		findTimeStep(permanentInteractions->getCurrent() , bodies);

	for( interactions->gotoFirst() ; interactions->notAtEnd() ; interactions->gotoNext() )
		findTimeStep(interactions->getCurrent() , bodies);

	if(! computedSomething) // no interactions at all? so let's try to estimate timestep by investigating bodies
		for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
		{
			Body* b = bodies->getCurrent().get();
			if( b->getGroup() == sdecGroup)
				findTimeStepFromBody(b);
		}
		
	if(computedSomething)
	{
		Omega::instance().setTimeStep(newDt);
		cerr << "SDECTimeStepper, timestep chosen is:" << Omega::instance().getTimeStep() << endl;
	}
}

