#include "FEMRock.hpp"

//#include "Rand.hpp" 
#include "Polyhedron.hpp"
#include "AABB.hpp"
#include "FEMBody.hpp"
#include "NonConnexBody.hpp"
//#include "SimpleSpringDynamicEngine.hpp"
//#include "ExplicitMassSpringDynamicEngine.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "Box.hpp"
#include "RigidBody.hpp"


FEMRock::FEMRock () : Serializable()
{
	exec();
}

FEMRock::~FEMRock ()
{

}

void FEMRock::processAttributes()
{
}

void FEMRock::registerAttributes()
{
}

void FEMRock::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);

	Quaternion q;

	q.fromAngleAxis(0, Vector3(0,0,1));
	
	//rootBody->dynamic	   = shared_ptr<DynamicEngine>(new SimpleSpringDynamicEngine);
	//rootBody->kinematic	   = shared_ptr<KinematicEngine>(new Rotor);
	//rootBody->broadCollider  = shared_ptr<BroadPhaseCollider>(new SAPCollider);
	//rootBody->narrowCollider  = shared_ptr<NarrowPhaseCollider>(new SimpleNarrowCollider);
	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3(0,0,0);
	rootBody->angularVelocity= Vector3(0,0,0);
	rootBody->se3		 = Se3(Vector3(0,0,0),q);

	shared_ptr<FEMBody> femBody(new FEMBody);
	shared_ptr<AABB> aabb(new AABB);	
	shared_ptr<Polyhedron> poly(new Polyhedron);
//	femBody->dynamic	= shared_ptr<DynamicEngine>(new ExplicitMassSpringDynamicEngine);
	femBody->isDynamic	= true;
	femBody->angularVelocity= Vector3(0,0,0);
	femBody->velocity	= Vector3(0,0,0);
	femBody->se3		= Se3(Vector3(0,0,0),q);	

	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,0);
//	aabb->halfSize		= Vector3((cellSize*(width-1))*0.5,0,(cellSize*(height-1))*0.5);
	femBody->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

/*
	for(unsigned int i=0;i<poly->edges.size();i++)
	{
		Vector3 v1 = poly->vertices[poly->edges[i].first];
		Vector3 v2 = poly->vertices[poly->edges[i].second];
		femBody->initialLengths.push_back((v1-v2).length());
	}
*/	
	poly->diffuseColor	= Vector3(0,0,1);
	poly->wire		= false;
	poly->visible		= true;
	poly->mshFileName 	= "../data/rock.msh";
	femBody->cm		= dynamic_pointer_cast<CollisionModel>(poly);
	femBody->gm		= dynamic_pointer_cast<CollisionModel>(poly);

	

	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(femBody));

	shared_ptr<Box> box;	
	shared_ptr<RigidBody> box1(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3(0,0,0);
	box1->velocity		= Vector3(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3(0,0,0);
	box1->se3		= Se3(Vector3(0,0,10),q);
	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,10);
	aabb->halfSize		= Vector3(50,5,40);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3(50,5,40);
	box->diffuseColor	= Vector3(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionModel>(box);
	box1->gm		= dynamic_pointer_cast<CollisionModel>(box);

	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(box1));

	
	IOManager::saveToFile("XMLManager", "../data/FEMRock.xml", "rootBody", rootBody);

}
