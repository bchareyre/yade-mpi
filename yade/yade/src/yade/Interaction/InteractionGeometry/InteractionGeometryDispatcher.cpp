#include "InteractionGeometryDispatcher.hpp"
#include "ComplexBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryDispatcher::postProcessAttributes(bool deserializing)
{
	postProcessDispatcher2D(deserializing);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryDispatcher::action(Body* body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& permanentInteractions = ncb->permanentInteractions;
	for( permanentInteractions->gotoFirstPotential() ; permanentInteractions->notAtEndPotential() ; permanentInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = permanentInteractions->getCurrent();
		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FIXME : those lines are dirty !	They were in SDECDynamicEngine, but they belong to this place		///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// FIXME - this is much shorter but still dirty (but now in different aspect - the way we store interactions)
		const shared_ptr<Interaction>& interaction2 = ncb->interactions->find(interaction->getId1(),interaction->getId2());
		if (interaction2)
			interaction2->isNonPermanent = false;
			
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// those lines are dirty !	END										///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = true;
		operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
	
	shared_ptr<InteractionContainer>& interactions = ncb->interactions;
	for( interactions->gotoFirstPotential() ; interactions->notAtEndPotential() ; interactions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = interactions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = interaction->isNonPermanent && operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
}
