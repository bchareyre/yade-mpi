#include "SimpleNarrowCollider.hpp"
#include "BVCFFinalCollider.hpp"
#include "Body.hpp"

SimpleNarrowCollider::SimpleNarrowCollider() : NarrowPhaseCollider()
{
	collider = shared_ptr<FinalCollider>(new BVCFFinalCollider());
}

SimpleNarrowCollider::~ SimpleNarrowCollider()
{

}

void SimpleNarrowCollider::processAttributes()
{

}

void SimpleNarrowCollider::registerAttributes()
{
}

bool SimpleNarrowCollider::narrowCollisionPhase(const std::vector<shared_ptr<Body> >& bodies, std::list<shared_ptr<Contact> >& contacts)
{
	std::list<shared_ptr<Contact> >::iterator it = contacts.begin();
	std::list<shared_ptr<Contact> >::iterator itEnd = contacts.end();
	std::list<shared_ptr<Contact> >::iterator itTmp;

	for(int i=0 ; it!=itEnd ; i++)
	{
		itTmp = it;
		it++;
		shared_ptr<Body> b1 = bodies[(*itTmp)->id1];
		shared_ptr<Body> b2 = bodies[(*itTmp)->id2];
		if (!(collider->collide(b1->cm,b2->cm,b1->se3,b2->se3,(*itTmp))))
		{
			//delete (*itTmp);
			contacts.erase(itTmp);
		}
	}

	return true;
}

