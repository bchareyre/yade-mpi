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

void FEMRock::afterDeserialization()
{
}

void FEMRock::registerAttributes()
{
}

void FEMRock::exec()
{
	shared_ptr<NonConnexBody> rootBody(new NonConnexBody);

	Quaternionr q;

	q.fromAxisAngle(Vector3r(0,0,1),0);

	//rootBody->dynamic	   = shared_ptr<DynamicEngine>(new SimpleSpringDynamicEngine);
	//rootBody->kinematic	   = shared_ptr<KinematicEngine>(new Rotor);
	//rootBody->broadCollider  = shared_ptr<BroadCollider>(new SAPCollider);
	//rootBody->narrowCollider  = shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3r(0,0,0);
	rootBody->angularVelocity= Vector3r(0,0,0);
	rootBody->se3		 = Se3r(Vector3r(0,0,0),q);

	shared_ptr<FEMBody> femBody(new FEMBody);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Polyhedron> poly(new Polyhedron);
//	femBody->dynamic	= shared_ptr<DynamicEngine>(new ExplicitMassSpringDynamicEngine);
	femBody->isDynamic	= true;
	femBody->angularVelocity= Vector3r(0,0,0);
	femBody->velocity	= Vector3r(0,0,0);
	femBody->se3		= Se3r(Vector3r(0,0,0),q);

	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,0);
//	aabb->halfSize		= Vector3r((cellSize*(width-1))*0.5,0,(cellSize*(height-1))*0.5);
	femBody->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

/*
	for(unsigned int i=0;i<poly->edges.size();i++)
	{
		Vector3r v1 = poly->vertices[poly->edges[i].first];
		Vector3r v2 = poly->vertices[poly->edges[i].second];
		femBody->initialLengths.push_back((v1-v2).length());
	}
*/
	poly->diffuseColor	= Vector3r(0,0,1);
	poly->wire		= false;
	poly->visible		= true;
	poly->mshFileName 	= "../data/rock.msh";
	femBody->cm		= dynamic_pointer_cast<CollisionGeometry>(poly);
	femBody->gm		= dynamic_pointer_cast<CollisionGeometry>(poly);



	shared_ptr<Body> b;
	b=dynamic_pointer_cast<Body>(femBody);
	rootBody->bodies->insert(b);

	shared_ptr<Box> box;
	shared_ptr<RigidBody> box1(new RigidBody);
	aabb=shared_ptr<AABB>(new AABB);
	box=shared_ptr<Box>(new Box);
	box1->isDynamic		= false;
	box1->angularVelocity	= Vector3r(0,0,0);
	box1->velocity		= Vector3r(0,0,0);
	box1->mass		= 0;
	box1->inertia		= Vector3r(0,0,0);
	box1->se3		= Se3r(Vector3r(0,0,10),q);
	aabb->color		= Vector3r(1,0,0);
	aabb->center		= Vector3r(0,0,10);
	aabb->halfSize		= Vector3r(50,5,40);
	box1->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	box->extents		= Vector3r(50,5,40);
	box->diffuseColor	= Vector3r(1,1,1);
	box->wire		= false;
	box->visible		= true;
	box1->cm		= dynamic_pointer_cast<CollisionGeometry>(box);
	box1->gm		= dynamic_pointer_cast<CollisionGeometry>(box);

	b = dynamic_pointer_cast<Body>(box1);
	rootBody->bodies->insert(b);


	IOManager::saveToFile("XMLManager", "../data/FEMRock.xml", "rootBody", rootBody);

}
