#include "ExplicitMassSpringDynamicEngine.hpp"
#include "Omega.hpp"
#include "Cloth.hpp"
#include "Mesh2D.hpp"

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


void ExplicitMassSpringDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >&)
{

	float dt = Omega::instance().dt;
	Cloth * cloth = dynamic_cast<Cloth*>(body);

	Vector3 gravity = Omega::instance().gravity;
	
	float damping	= cloth->damping;
	float stiffness	= cloth->stiffness;

	shared_ptr<Mesh2D> mesh = dynamic_pointer_cast<Mesh2D>(cloth->gm);
	vector<Vector3>& vertices = mesh->vertices;
	vector<Edge>& edges	  = mesh->edges;

	if (first)
	{		
		forces.resize(vertices.size());		
		prevVelocities.resize(vertices.size());
		/*vector<NodeProperties>::iterator pi = cloth->properties.begin();
		vector<NodeProperties>::iterator piEnd = cloth->properties.end();
		vector<Vector3>::iterator pvi = prevVelocities.begin();
		for( ; pi!=piEnd ; ++pi,++pvi)
			*pvi = (*pi).velocity;*/
	}

	std::fill(forces.begin(),forces.end(),Vector3(0,0,0));
	
	vector<Edge>::iterator ei = edges.begin();
	vector<Edge>::iterator eiEnd = edges.end();
	for(int i=0 ; ei!=eiEnd; ++ei,i++)
	{
		Vector3 v1 = vertices[(*ei).first];
		Vector3 v2 = vertices[(*ei).second];
		float l  = (v2-v1).length();
		float l0 = cloth->initialLengths[i];		
		Vector3 dir = (v2-v1).normalize();
		float e  = (l-l0)/l0;
		float relativeVelocity = dir.dot((cloth->properties[(*ei).second].velocity-cloth->properties[(*ei).first].velocity));
		Vector3 f3 = (e*stiffness+relativeVelocity*damping)*dir;
		forces[(*ei).first]  += f3;
		forces[(*ei).second] -= f3;
	}

		
	for(unsigned int i=0; i < vertices.size(); i++)
        {
		Vector3 acc = Vector3(0,0,0);

		if (cloth->properties[i].invMass!=0)
			acc = Omega::instance().gravity + forces[i]*cloth->properties[i].invMass;
					
		if (!first)
			cloth->properties[i].velocity = 0.999*(prevVelocities[i]+0.5*dt*acc);
		
		prevVelocities[i] = (cloth->properties[i].velocity+0.5*dt*acc);
		
		vertices[i] += prevVelocities[i]*dt;				
        }
	
	// FIXME: where should we update bounding volume
	body->updateBoundingVolume(body->se3);
	first = false;
	

}

