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
#include "SDECContactPhysics.hpp"
#include "SDECContactGeometry.hpp"
#include "Sphere.hpp"
#include "SDECMacroMicroElasticRelationships.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SDECTimeStepper::SDECTimeStepper() : Actor() , sdecContactModel(new SDECMacroMicroElasticRelationships)
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

	Real Dab  	= sphere->radius;
	Real rad3 	= std::pow(Dab,2); // radius to the power of 2, from sphere

	Real Eab 	= sdec->young;
	Real Vab 	= sdec->poisson;
	Real Dinit 	= 2*Dab; // assuming thet sphere is in contact with itself
	Real Sinit 	= Mathr::PI * std::pow( Dab , 2);

	Real alpha 	= sdecContactModel->alpha;
	Real beta 	= sdecContactModel->beta;
	Real gamma 	= sdecContactModel->gamma;

	Real Kn		= abs((Eab*Sinit/Dinit)*( (1+alpha)/(beta*(1+Vab) + gamma*(1-alpha*Dab) ) ));
	Real Ks		= abs(Kn*(1-alpha*Vab)/(1+Vab));

	Real dt = 0.1*min(
			  sqrt( sdec->mass       /  Kn       )
			, sqrt( sdec->inertia[0] / (Ks*rad3) )
		  );

	newDt = std::min(dt,newDt);
	computedSomething = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECTimeStepper::findTimeStepFromInteraction(const shared_ptr<Interaction>& interaction, shared_ptr<BodyContainer>& bodies)
{
	unsigned int id1 = interaction->getId1();
	unsigned int id2 = interaction->getId2();
		
	if( (*bodies)[id1]->getGroup() != sdecGroup || (*bodies)[id2]->getGroup() != sdecGroup )
		return; // skip other groups

	SDECContactPhysics* sdecContact = dynamic_cast<SDECContactPhysics*>(interaction->interactionPhysics.get());
	SDECContactGeometry* interactionGeometry = dynamic_cast<SDECContactGeometry*>(interaction->interactionGeometry.get());
	SDECParameters * body1	= dynamic_cast<SDECParameters*>((*bodies)[id1]->physicalParameters.get());
	SDECParameters * body2	= dynamic_cast<SDECParameters*>((*bodies)[id2]->physicalParameters.get());

	if(! (sdecContact && interactionGeometry && body1 && body2))
		return;
	
	Real mass 	= std::min( body1->mass                   , body2->mass               );
//	if(mass == 0) 			// FIXME - remove that comment: zero mass and zero inertia are too stupid to waste time checking that.
//		mass 	= std::max( body1->mass                   , body2->mass               );
//	if(mass == 0)
//		return; // not possible to compute
	Real inertia 	= std::min( body1->inertia[0]             , body2->inertia[0]         );
//	if( inertia == 0)
//		inertia = std::max( body1->inertia[0]             , body2->inertia[0]         );
//	if( inertia == 0)
//		return;
	Real rad3 	= std::pow( std::max(interactionGeometry->radius1 , interactionGeometry->radius2 ) , 2); // radius to the power of 2, from sphere

	Real dt = 0.1*min(
			  sqrt( mass     / abs(sdecContact->kn)      )
			, sqrt( inertia  / abs(sdecContact->ks*rad3) )
		  );

	newDt = std::min(dt,newDt);
	computedSomething = true;
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
		findTimeStepFromInteraction(permanentInteractions->getCurrent() , bodies);

	for( interactions->gotoFirst() ; interactions->notAtEnd() ; interactions->gotoNext() )
		findTimeStepFromInteraction(interactions->getCurrent() , bodies);

	if(! computedSomething)
// no interactions at all? so let's try to estimate timestep by investigating bodies,
// simulating that a body in contact with itself. this happens only when there were not interactions at all.
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

