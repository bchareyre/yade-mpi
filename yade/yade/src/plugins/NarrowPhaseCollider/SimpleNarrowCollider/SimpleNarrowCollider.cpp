#include "SimpleNarrowCollider.hpp"
#include "Contact.hpp"
#include "Body.hpp"
//#include "Omega.hpp"

SimpleNarrowCollider::SimpleNarrowCollider() : NarrowCollider()
{
	//collider = shared_ptr<FinalCollider>(new BVCFFinalCollider());
}

SimpleNarrowCollider::~ SimpleNarrowCollider()
{

}

void SimpleNarrowCollider::processAttributes()
{
	NarrowCollider::processAttributes();
}

void SimpleNarrowCollider::registerAttributes()
{
	NarrowCollider::registerAttributes();
}

bool SimpleNarrowCollider::narrowCollisionPhase(const std::vector<shared_ptr<Body> >& bodies, std::list<shared_ptr<Interaction> >& interactions)
{
	std::list<shared_ptr<Interaction> >::iterator it = interactions.begin();
	std::list<shared_ptr<Interaction> >::iterator itEnd = interactions.end();
	shared_ptr<Contact> contact;
	std::list<shared_ptr<Interaction> >::iterator itTmp;
	
	for(int i=0 ; it!=itEnd ; i++)
	{
		itTmp = it;
		contact = static_pointer_cast<Contact>(*itTmp);
		
		it++;
		shared_ptr<Body> b1 = bodies[contact->id1];
		shared_ptr<Body> b2 = bodies[contact->id2];
		
		//if (!(Omega::instance().narrowCollider.go(b1->cm,b2->cm,b1->se3,b2->se3,(*itTmp))))
		if (!(narrowManager.collide(b1->cm,b2->cm,b1->se3,b2->se3,(*itTmp))))
			interactions.erase(itTmp);
	}

	return true;
}

