#include "SimpleSpringDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "ClosestFeatures.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"

SimpleSpringDynamicEngine::SimpleSpringDynamicEngine () : DynamicEngine()
{
	first = true;
}

SimpleSpringDynamicEngine::~SimpleSpringDynamicEngine ()
{

}

void SimpleSpringDynamicEngine::afterDeserialization()
{

}

void SimpleSpringDynamicEngine::registerAttributes()
{
}


void SimpleSpringDynamicEngine::respondToCollisions(Body * body)
{

	//float dt = Omega::instance().dt;

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;

	float stiffness = 10000;
	float viscosity = 10;
	Vector3r gravity = Omega::instance().getGravity();
	if (first)
	{
		forces.resize(bodies.size());
		couples.resize(bodies.size());
		prevVelocities.resize(bodies.size());
	}

	fill(forces.begin(),forces.end(),Vector3r(0,0,0));
	fill(couples.begin(),couples.end(),Vector3r(0,0,0));

	std::list<shared_ptr<Interaction> >::const_iterator cti = ncb->interactions.begin();
	std::list<shared_ptr<Interaction> >::const_iterator ctiEnd = ncb->interactions.end();
	for( ; cti!=ctiEnd ; ++cti)
	{
		shared_ptr<Interaction> contact = (*cti);
		shared_ptr<RigidBody> rb1 = dynamic_pointer_cast<RigidBody>(bodies[contact->getId1()]);
		shared_ptr<RigidBody> rb2 = dynamic_pointer_cast<RigidBody>(bodies[contact->getId2()]);

		std::vector<std::pair<Vector3r,Vector3r> >::iterator cpi = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.begin();
		std::vector<std::pair<Vector3r,Vector3r> >::iterator cpiEnd = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.end();
		float size = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.size();
		for( ; cpi!=cpiEnd ; ++cpi)
		{
			Vector3r p1 = (*cpi).first;
			Vector3r p2 = (*cpi).second;

			Vector3r p = 0.5*(p1+p2);

			Vector3r o1p = (p - rb1->se3.translation);
			Vector3r o2p = (p - rb2->se3.translation);

			Vector3r dir = p2-p1;
			float l  = dir.normalize();
			float elongation  = l*l;

			//Vector3r v1 = rb1->velocity+o1p.cross(rb1->angularVelocity);
			//Vector3r v2 = rb2->velocity+o2p.cross(rb2->angularVelocity);
			Vector3r v1 = rb1->velocity+rb1->angularVelocity.cross(o1p);
			Vector3r v2 = rb2->velocity+rb2->angularVelocity.cross(o2p);
			float relativeVelocity = dir.dot(v2-v1);
			Vector3r f = (elongation*stiffness+relativeVelocity*viscosity)/size*dir;

			forces[contact->getId1()] += f;
			forces[contact->getId2()] -= f;

			couples[contact->getId1()] += o1p.cross(f);
			couples[contact->getId2()] -= o2p.cross(f);
		}
	}


	for(unsigned int i=0; i < bodies.size(); i++)
        {
		shared_ptr<RigidBody> rb = dynamic_pointer_cast<RigidBody>(bodies[i]);

		if (rb)
		{
			rb->acceleration += forces[i]*rb->invMass;
			rb->angularAcceleration += couples[i].multDiag(rb->invInertia);
			//if (i==35)
			//	cerr << rb->acceleration << "||" << rb->angularAcceleration << endl;
		}
        }

	first = false;
}

