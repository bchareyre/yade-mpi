#include "SimpleNarrowCollider.hpp"
#include "Body.hpp"
#include "NonConnexBody.hpp"

SimpleNarrowCollider::SimpleNarrowCollider() : NarrowCollider()
{
}

SimpleNarrowCollider::~ SimpleNarrowCollider()
{

}

void SimpleNarrowCollider::postProcessAttributes(bool deserializing)
{
	NarrowCollider::postProcessAttributes(deserializing);
}

void SimpleNarrowCollider::registerAttributes()
{
	NarrowCollider::registerAttributes();
}

void SimpleNarrowCollider::narrowCollisionPhase(Body* body)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> contact;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		contact = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[contact->getId1()];
		shared_ptr<Body> b2 = (*bodies)[contact->getId2()];

		contact->isReal = narrowCollisionDispatcher( b1->cm , b2->cm , b1->se3 , b2->se3 , contact );
	}
}

