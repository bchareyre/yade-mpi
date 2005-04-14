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
	
	shared_ptr<InteractionContainer>& initialInteractions = ncb->initialInteractions;
	for( initialInteractions->gotoFirstPotential() ; initialInteractions->notAtEndPotential() ; initialInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = initialInteractions->getCurrent();
		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FIXME : those lines are dirty !	They were in ElasticContactLaw, but they belong to this place		///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// FIXME - this is much shorter but still dirty (but now in different aspect - the way we store runtimeInteractions)
		const shared_ptr<Interaction>& interaction2 = ncb->runtimeInteractions->find(interaction->getId1(),interaction->getId2());
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
	
	shared_ptr<InteractionContainer>& runtimeInteractions = ncb->runtimeInteractions;
	for( runtimeInteractions->gotoFirstPotential() ; runtimeInteractions->notAtEndPotential() ; runtimeInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = runtimeInteractions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = interaction->isNonPermanent && operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
}
