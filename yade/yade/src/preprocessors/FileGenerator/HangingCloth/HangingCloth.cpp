#include "HangingCloth.hpp"

#include "Rand.hpp" 
#include "Mesh2D.hpp"
#include "AABB.hpp"
#include "NonConnexBody.hpp"
//#include "SimpleSpringDynamicEngine.hpp"
#include "SAPCollider.hpp"
//#include "Rotor.hpp"
#include "SimpleNarrowCollider.hpp"
#include "Cloth.hpp"

#include <fstream>
#include "IOManager.hpp"

HangingCloth::HangingCloth () : Serializable()
{
	exec();
}

HangingCloth::~HangingCloth ()
{

}

void HangingCloth::processAttributes()
{
}

void HangingCloth::registerAttributes()
{
}

void HangingCloth::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);
	int sqrtNbNodes = 10;
	float mass = 10;
	Quaternion q;
	
	q.fromAngleAxis(0, Vector3(0,0,1));
	
	//rootBody->dynamic	   = shared_ptr<DynamicEngine>(new SimpleSpringDynamicEngine);
	//rootBody->kinematic	   = shared_ptr<KinematicEngine>(new Rotor);
	rootBody->broadCollider	   = shared_ptr<BroadPhaseCollider>(new SAPCollider);
	rootBody->narrowCollider   = shared_ptr<NarrowPhaseCollider>(new SimpleNarrowCollider);
	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3(0,0,0);
	rootBody->angularVelocity= Vector3(0,0,0);
	rootBody->se3		 = Se3(Vector3(0,0,0),q);

	//for(int i=0;i<7;i++)
	//	rootBody->kinematic->subscribedBodies.push_back(i);

	shared_ptr<Cloth> cloth(new Cloth);	
	shared_ptr<AABB> aabb(new AABB);	
	shared_ptr<Mesh2D> mesh2d(new Mesh2D);

	cloth->isDynamic	= true;
	cloth->angularVelocity	= Vector3(0,0,0);
	cloth->velocity		= Vector3(0,0,0);
	cloth->mass		= mass;
	cloth->se3		= Se3(Vector3(0,0,10),q);
	cloth->stiffness	= 100;
	cloth->damping		= 10;
	for(int i=0;i<sqrtNbNodes*sqrtNbNodes;i++)
		cloth->properties.push_back(NodeProperties(mass/(float)(sqrtNbNodes*sqrtNbNodes)));
	
	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,10);
	aabb->halfSize		= Vector3(50,5,40);
	cloth->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

	for(int i=0;i<sqrtNbNodes;i++)
		for(int j=0;j<sqrtNbNodes;j++)
		mesh2d->vertices.push_back(Vector3(i*2,j*2,0));

	for(int i=0;i<sqrtNbNodes-1;i++)
		for(int j=0;j<sqrtNbNodes-1;j++)
		{					
			mesh2d->edges.push_back(Edge(i+j*sqrtNbNodes,i+1+j*sqrtNbNodes));
			mesh2d->edges.push_back(Edge(i+j*sqrtNbNodes,i+(j+1)*sqrtNbNodes));
			mesh2d->edges.push_back(Edge(i+j*sqrtNbNodes,i+1+(j+1)*sqrtNbNodes));
		}
		
	for(int i=0;i<sqrtNbNodes-1;i++)
		mesh2d->edges.push_back(Edge(i+(sqrtNbNodes-1)*sqrtNbNodes,i+1+(sqrtNbNodes-1)*sqrtNbNodes));

	for(int j=0;j<sqrtNbNodes-1;j++)
		mesh2d->edges.push_back(Edge(sqrtNbNodes-1+j*sqrtNbNodes,sqrtNbNodes-1+(j+1)*sqrtNbNodes));
		
	mesh2d->diffuseColor	= Vector3(1,1,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	cloth->cm		= dynamic_pointer_cast<CollisionModel>(mesh2d);
	cloth->gm		= dynamic_pointer_cast<CollisionModel>(mesh2d);


	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(cloth));

	IOManager::saveToFile("XMLManager", "../data/scene.xml", "rootBody", rootBody);

}
