#include "ExplicitMassSpringDynamicEngine.hpp"
#include "Omega.hpp"
#include "Cloth.hpp"

ExplicitMassSpringDynamicEngine::ExplicitMassSpringDynamicEngine () : DynamicEngine()
{
	first = true;
}

ExplicitMassSpringDynamicEngine::~ExplicitMassSpringDynamicEngine ()
{

}

void ExplicitMassSpringDynamicEngine::processAttributes()
{

}

void ExplicitMassSpringDynamicEngine::registerAttributes()
{
}


void ExplicitMassSpringDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions,float dt)
{

	Cloth * cloth = dynamic_cast<Cloth*>(body);

	Vector3 gravity = Omega::instance().gravity;
	
	float damping	= cloth->damping;
	float stiffness	= cloth->stiffness;
	
	/*if (first)
	{
		forces.resize(cloth->gm->edges.size());
		prevVelocities.resize(cloth->gm->edges.size());
	}
	
	std::vector<Vector3>::iterator fi = forces.begin();
	std::vector<Vector3>::iterator fiEnd = forces.end();
	vector<Edge>::iterator ei = cloth->gm->edges.begin();
	for( ; fi!=fiEnd; ++fi, ++ei)
	{
		Vector3 v1 = cloth->gm->vertices[(*ei).first];
		Vector3 v2 = cloth->gm->vertices[(*ei).second];
		(*fi) = (v2-v1).normalize();
		
		(*fi) += gravity*mass;
	}
	
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

		if (rb->isDynamic)
		{
			Vector3 acc = forces[i]*rb->invMass;

			if (!first)
				rb->velocity = prevVelocities[i]+0.5*dt*acc;

			prevVelocities[i] = rb->velocity+0.5*dt*acc;
			rb->se3.translation += prevVelocities[i]*dt;

			rb->updateBoundingVolume(rb->se3);
		}
        }*/
	first = false;

}

