#include "MassSpringBody2RigidBodyDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "MassSpringBody.hpp"
#include "Contact.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"
#include "ClosestFeatures.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "Mesh2D.hpp"

MassSpringBody2RigidBodyDynamicEngine::MassSpringBody2RigidBodyDynamicEngine () : DynamicEngine()
{
}

MassSpringBody2RigidBodyDynamicEngine::~MassSpringBody2RigidBodyDynamicEngine ()
{

}

void MassSpringBody2RigidBodyDynamicEngine::processAttributes()
{

}

void MassSpringBody2RigidBodyDynamicEngine::registerAttributes()
{
}


void MassSpringBody2RigidBodyDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions)
{	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	std::list<shared_ptr<Interaction> > tmpI;
	list<shared_ptr<Interaction> >::const_iterator ii = interactions.begin();
	list<shared_ptr<Interaction> >::const_iterator iiEnd = interactions.end();
	
	for( ; ii!=iiEnd ; ++ii)
	{
		shared_ptr<Contact> ct = static_pointer_cast<Contact>(*ii);
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(ct->interactionModel);
		//FIXME : this is a hack because we don't know if id1 is the sphere or piece of massSpring
 		shared_ptr<MassSpringBody> c = dynamic_pointer_cast<MassSpringBody>(ncb->bodies[ct->id1]);
		shared_ptr<RigidBody> rb = dynamic_pointer_cast<RigidBody>(ncb->bodies[ct->id2]);
		shared_ptr<Mesh2D> mesh;
		if (c)
		{
			mesh = dynamic_pointer_cast<Mesh2D>(c->gm);
			for(unsigned int i=0;i<cf->verticesId.size();i++)
			{
				
				Vector3 p1 = cf->closestsPoints[i].first;
				Vector3 p2 = cf->closestsPoints[i].second;
				Vector3 dir = p2-p1;
				float l = dir.unitize();
				float relativeVelocity = dir.dot(rb->velocity);
				float fi = 0.1*l*l/3.0+relativeVelocity*10;
				Vector3 f = fi*dir;
				rb->acceleration -= f*rb->invMass;
				
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][0],f));
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][1],f));
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][2],f));
			}		
		}
		else //if (cf->verticesId.size()==0)
			tmpI.push_back(*ii);
	}

	shared_ptr<DynamicEngine> de = dynamic_pointer_cast<DynamicEngine>(ClassFactory::instance().createShared("SimpleSpringDynamicEngine"));

	de->respondToCollisions(body,tmpI);

}

