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
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> interaction;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		interaction = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body> b2 = (*bodies)[interaction->getId2()];

		interaction->isReal = operator()( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
}
