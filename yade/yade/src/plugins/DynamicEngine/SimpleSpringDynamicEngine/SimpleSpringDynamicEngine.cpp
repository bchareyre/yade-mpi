#include "SimpleSpringDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "ClosestFeatures.hpp"
#include "Omega.hpp"
#include "Contact.hpp"
#include "NonConnexBody.hpp"

SimpleSpringDynamicEngine::SimpleSpringDynamicEngine () : DynamicEngine()
{
	first = true;
}

SimpleSpringDynamicEngine::~SimpleSpringDynamicEngine ()
{

}

void SimpleSpringDynamicEngine::processAttributes()
{

}

void SimpleSpringDynamicEngine::registerAttributes()
{
}


void SimpleSpringDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions)
{

	//float dt = Omega::instance().dt;
	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;
	
	float stiffness = 100;
	//float damping = 1.02;
	Vector3 gravity = Omega::instance().gravity;
	if (first)
	{
		forces.resize(bodies.size());
		couples.resize(bodies.size());
		prevVelocities.resize(bodies.size());
	}
	
	fill(forces.begin(),forces.end(),Vector3(0,0,0));
	fill(couples.begin(),couples.end(),Vector3(0,0,0));
	
	std::list<shared_ptr<Interaction> >::const_iterator cti = interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{
		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);
		
		std::vector<std::pair<Vector3,Vector3> >::iterator cpi = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.begin();
		std::vector<std::pair<Vector3,Vector3> >::iterator cpiEnd = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.end();
		for( ; cpi!=cpiEnd ; ++cpi)
		{	
			Vector3 f = (*cpi).first-(*cpi).second;
			
			f *= stiffness/(float)((dynamic_pointer_cast<ClosestFeatures>(contact->interactionModel))->closestsPoints.size());
			
			Vector3 p = 0.5*((*cpi).first+(*cpi).second);
			
			shared_ptr<RigidBody> r1 = dynamic_pointer_cast<RigidBody>(bodies[contact->id1]);
			shared_ptr<RigidBody> r2 = dynamic_pointer_cast<RigidBody>(bodies[contact->id2]);
			
			Vector3 o1p = p - r1->se3.translation;
			Vector3 o2p = p - r2->se3.translation;
						
			forces[contact->id1] -= f;
			forces[contact->id2] += f;
		
			couples[contact->id1] -= o1p.cross(f);
			couples[contact->id2] += o2p.cross(f);
		}
	}

		
	for(unsigned int i=0; i < bodies.size(); i++)
        {
		shared_ptr<RigidBody> rb = dynamic_pointer_cast<RigidBody>(bodies[i]);

		if (rb)
			rb->acceleration += forces[i]*rb->invMass;
        }
	
	first = false;
}

