#include "SimpleSpringDynamicEngine.hpp"
#include "RigidBodyParameters.hpp"
#include "ClosestFeatures.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"
#include "ActionForce.hpp"
#include "ActionMomentum.hpp"

SimpleSpringDynamicEngine::SimpleSpringDynamicEngine () : DynamicEngine(), actionForce(new ActionForce) , actionMomentum(new ActionMomentum)
{
	first = true;
}

void SimpleSpringDynamicEngine::registerAttributes()
{
}


void SimpleSpringDynamicEngine::respondToInteractions(Body * body)
{

	//Real dt = Omega::instance().dt;

	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Real stiffness = 10000;
	Real viscosity = 10;
	Vector3r gravity = Omega::instance().getGravity();
	
	if(first) // FIXME - this should be done somewhere else
	{
		vector<shared_ptr<Action> > vvv; 
		vvv.clear();
		vvv.push_back(actionForce);
		vvv.push_back(actionMomentum);
		ncb->actions->prepare(vvv);
	}

	for( ncb->interactions->gotoFirst() ; ncb->interactions->notAtEnd() ; ncb->interactions->gotoNext() )
	{
		const shared_ptr<Interaction>& contact = ncb->interactions->getCurrent();
		int id1 = contact->getId1();
		int id2 = contact->getId2();

		shared_ptr<RigidBodyParameters> rb1 = dynamic_pointer_cast<RigidBodyParameters>((*bodies)[id1]->physicalParameters);
		shared_ptr<RigidBodyParameters> rb2 = dynamic_pointer_cast<RigidBodyParameters>((*bodies)[id2]->physicalParameters);

		std::vector<std::pair<Vector3r,Vector3r> >::iterator cpi = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.begin();
		std::vector<std::pair<Vector3r,Vector3r> >::iterator cpiEnd = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.end();
		Real size = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.size();
		for( ; cpi!=cpiEnd ; ++cpi)
		{
			Vector3r p1 = (*cpi).first;
			Vector3r p2 = (*cpi).second;

			Vector3r p = 0.5*(p1+p2);

			Vector3r o1p = (p - rb1->se3.translation);
			Vector3r o2p = (p - rb2->se3.translation);

			Vector3r dir = p2-p1;
			Real l  = dir.normalize();
			Real elongation  = l*l;

			//Vector3r v1 = rb1->velocity+o1p.cross(rb1->angularVelocity);
			//Vector3r v2 = rb2->velocity+o2p.cross(rb2->angularVelocity);
			Vector3r v1 = rb1->velocity+rb1->angularVelocity.cross(o1p);
			Vector3r v2 = rb2->velocity+rb2->angularVelocity.cross(o2p);
			Real relativeVelocity = dir.dot(v2-v1);
			Vector3r f = (elongation*stiffness+relativeVelocity*viscosity)/size*dir;

			static_cast<ActionForce*>   ( ncb->actions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    += f;
			static_cast<ActionForce*>   ( ncb->actions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    -= f;
		
			static_cast<ActionMomentum*>( ncb->actions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum += o1p.cross(f);
			static_cast<ActionMomentum*>( ncb->actions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum -= o2p.cross(f);
			
		}
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Gravity														///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		shared_ptr<Body>& b = bodies->getCurrent();
		RigidBodyParameters * de = static_cast<RigidBodyParameters*>(b->physicalParameters.get());
		static_cast<ActionForce*>( ncb->actions->find( b->getId() , actionForce->getClassIndex() ).get() )->force += gravity*de->mass;
        }

	first = false;
}

