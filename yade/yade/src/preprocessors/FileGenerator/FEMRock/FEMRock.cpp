#include "FEMRock.hpp"

//#include "Rand.hpp"
#include "Polyhedron.hpp"
#include "AABB.hpp"
#include "FEMNodeParameters.hpp"
#include "ComplexBody.hpp"
//#include "SimpleSpringLaw.hpp"
//#include "MassSpringLaw.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "Box.hpp"
#include "RigidBodyParameters.hpp"


FEMRock::FEMRock () : FileGenerator()
{

}

FEMRock::~FEMRock ()
{

}

void FEMRock::postProcessAttributes(bool)
{
}

void FEMRock::registerAttributes()
{
}

string FEMRock::generate()
{
// 	// FIXME : not working
// 	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
// 
// 	Quaternionr q;
// 
// 	q.fromAxisAngle(Vector3r(0,0,1),0);
// 
// 	//rootBody->dynamic	   = shared_ptr<ConstitutiveLaw>(new SimpleSpringLaw);
// 	//rootBody->kinematic	   = shared_ptr<Condition>(new RotationCondition);
// 	//rootBody->broadCollider  = shared_ptr<BroadInteractor>(new SAPCollider);
// 	//rootBody->narrowCollider  = shared_ptr<InteractionGeometryDispatcher>(new SimpleNarrowCollider);
// 	rootBody->isDynamic      = false;
// 	rootBody->velocity       = Vector3r(0,0,0);
// 	rootBody->angularVelocity= Vector3r(0,0,0);
// 	rootBody->se3		 = Se3r(Vector3r(0,0,0),q);
// 
// 	shared_ptr<FEMNodeParameters> femBody(new FEMNodeParameters);
// 	shared_ptr<AABB> aabb(new AABB);
// 	shared_ptr<Polyhedron> poly(new Polyhedron);
// 	//	femBody->dynamic	= shared_ptr<ConstitutiveLaw>(new MassSpringLaw);
// 	femBody->isDynamic	= true;
// 	femBody->angularVelocity= Vector3r(0,0,0);
// 	femBody->velocity	= Vector3r(0,0,0);
// 	femBody->se3		= Se3r(Vector3r(0,0,0),q);
// 
// 	aabb->color		= Vector3r(1,0,0);
// 	aabb->center		= Vector3r(0,0,0);
// //	aabb->halfSize		= Vector3r((cellSize*(width-1))*0.5,0,(cellSize*(height-1))*0.5);
// 	femBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
// 
// /*
// 	for(unsigned int i=0;i<poly->edges.size();i++)
// 	{
// 		Vector3r v1 = poly->vertices[poly->edges[i].first];
// 		Vector3r v2 = poly->vertices[poly->edges[i].second];
// 		femBody->initialLengths.push_back((v1-v2).length());
// 	}
// */
// 	poly->diffuseColor	= Vector3f(0,0,1);
// 	poly->wire		= false;
// 	poly->visible		= true;
// 	poly->mshFileName 	= "../data/rock.msh";
// 	femBody->interactionGeometry		= dynamic_pointer_cast<InteractionDescription>(poly);
// 	femBody->geometricalModel		= dynamic_pointer_cast<InteractionDescription>(poly);
// 
// 
// 
// 	shared_ptr<Body> b;
// 	b=dynamic_pointer_cast<Body>(femBody);
// 	rootBody->bodies->insert(b);
// 
// 	shared_ptr<Box> box;
// 	shared_ptr<RigidBodyParameters> box1(new RigidBodyParameters);
// 	aabb=shared_ptr<AABB>(new AABB);
// 	box=shared_ptr<Box>(new Box);
// 	box1->isDynamic		= false;
// 	box1->angularVelocity	= Vector3r(0,0,0);
// 	box1->velocity		= Vector3r(0,0,0);
// 	box1->mass		= 0;
// 	box1->inertia		= Vector3r(0,0,0);
// 	box1->se3		= Se3r(Vector3r(0,0,10),q);
// 	aabb->color		= Vector3r(1,0,0);
// 	aabb->center		= Vector3r(0,0,10);
// 	aabb->halfSize		= Vector3r(50,5,40);
// 	box1->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
// 	box->extents		= Vector3r(50,5,40);
// 	box->diffuseColor	= Vector3f(1,1,1);
// 	box->wire		= false;
// 	box->visible		= true;
// 	box1->interactionGeometry		= dynamic_pointer_cast<InteractionDescription>(box);
// 	box1->geometricalModel		= dynamic_pointer_cast<InteractionDescription>(box);
// 
// 	b = dynamic_pointer_cast<Body>(box1);
// 	rootBody->bodies->insert(b);


	return "";

}
