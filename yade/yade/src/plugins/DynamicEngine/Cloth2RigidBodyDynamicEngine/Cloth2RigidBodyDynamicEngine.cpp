#include "Cloth2RigidBodyDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "Cloth.hpp"
#include "Contact.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"
#include "ClosestFeatures.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "Mesh2D.hpp"

Cloth2RigidBodyDynamicEngine::Cloth2RigidBodyDynamicEngine () : DynamicEngine()
{
}

Cloth2RigidBodyDynamicEngine::~Cloth2RigidBodyDynamicEngine ()
{

}

void Cloth2RigidBodyDynamicEngine::processAttributes()
{

}

void Cloth2RigidBodyDynamicEngine::registerAttributes()
{
}


void Cloth2RigidBodyDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions)
{	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	std::list<shared_ptr<Interaction> > tmpI;
	list<shared_ptr<Interaction> >::const_iterator ii = interactions.begin();
	list<shared_ptr<Interaction> >::const_iterator iiEnd = interactions.end();
	
	for( ; ii!=iiEnd ; ++ii)
	{
		shared_ptr<Contact> ct = static_pointer_cast<Contact>(*ii);
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(ct->interactionModel);
		//FIXME : this is a hack because we don't know if id1 is the sphere or piece of cloth
 		shared_ptr<Cloth> c = dynamic_pointer_cast<Cloth>(ncb->bodies[ct->id1]);
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
				//FIXME :  put stiffness as a parameter of the engine
				float fi = 500*l;
				Vector3 f = (fi*dir)/cf->verticesId.size();
				rb->acceleration -= f*rb->invMass;
				
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][0],f/3.0));
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][1],f/3.0));
				c->externalForces.push_back(pair<int,Vector3>(mesh->faces[cf->verticesId[i]][2],f/3.0));
			}		
		}
		else //if (cf->verticesId.size()==0)
			tmpI.push_back(*ii);
	}

	shared_ptr<DynamicEngine> de = dynamic_pointer_cast<DynamicEngine>(ClassFactory::instance().createShared("SimpleSpringDynamicEngine"));

	de->respondToCollisions(body,tmpI);

}

