#include "InteractionGeometryMetaEngine.hpp"
#include <yade/MetaBody.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryMetaEngine::postProcessAttributes(bool deserializing)
{
	postProcessDispatcher2D(deserializing);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryMetaEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(functorNames);
	REGISTER_ATTRIBUTE(functorArguments);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometryMetaEngine::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
	for( persistentInteractions->gotoFirstPotential() ; persistentInteractions->notAtEndPotential() ; persistentInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = persistentInteractions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = true;
		operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
	
	shared_ptr<InteractionContainer>& volatileInteractions = ncb->volatileInteractions;
	for( volatileInteractions->gotoFirstPotential() ; volatileInteractions->notAtEndPotential() ; volatileInteractions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = volatileInteractions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		
		interaction->isReal =

		// FIXME put this inside VolatileInteractionCriterion dynlib
			( persistentInteractions->find(interaction->getId1(),interaction->getId2()) == 0 )

			
		 	&&
			operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
			
	}
}
