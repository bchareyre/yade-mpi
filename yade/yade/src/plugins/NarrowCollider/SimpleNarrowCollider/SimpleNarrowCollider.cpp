#include "SimpleNarrowCollider.hpp"
#include "Contact.hpp"
#include "Body.hpp"
#include "NonConnexBody.hpp"

SimpleNarrowCollider::SimpleNarrowCollider() : NarrowCollider()
{
}

SimpleNarrowCollider::~ SimpleNarrowCollider()
{

}

void SimpleNarrowCollider::afterDeserialization()
{
	NarrowCollider::afterDeserialization();
}

void SimpleNarrowCollider::registerAttributes()
{
	NarrowCollider::registerAttributes();
}

void SimpleNarrowCollider::narrowCollisionPhase(Body* body)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;

	std::list<shared_ptr<Interaction> >::iterator it = ncb->interactions.begin();
	std::list<shared_ptr<Interaction> >::iterator itEnd = ncb->interactions.end();
	shared_ptr<Contact> contact;
	std::list<shared_ptr<Interaction> >::iterator itTmp;
	
	for(int i=0 ; it!=itEnd ; i++)
	{
		itTmp = it;
		contact = static_pointer_cast<Contact>(*itTmp);
		
		it++;
		shared_ptr<Body> b1 = bodies[contact->id1];
		shared_ptr<Body> b2 = bodies[contact->id2];
		
		if (!(narrowManager.collide(b1->cm,b2->cm,b1->se3,b2->se3,(*itTmp))))
			ncb->interactions.erase(itTmp);
	}
}

