#include "HangingCloth.hpp"

#include "Rand.hpp"
#include "Mesh2D.hpp"
#include "Sphere.hpp"
#include "RigidBody.hpp"
#include "AABB.hpp"
#include "NonConnexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "BallisticDynamicEngine.hpp"
#include "SAPCollider.hpp"
#include "SimpleNarrowCollider.hpp"
#include "MassSpringBody.hpp"
#include "ExplicitMassSpringDynamicEngine.hpp"
#include "MassSpringBody2RigidBodyDynamicEngine.hpp"
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
	int width = 20;
	int height = 20;
	float mass = 10;
	const int cellSize = 20;
	Quaternion q;
	int nbSpheres = 10;
	q.fromAngleAxis(0, Vector3(0,0,1));

	shared_ptr<NarrowCollider> nc	= shared_ptr<NarrowCollider>(new SimpleNarrowCollider);
	nc->addCollisionFunctor("Sphere","Sphere","Sphere2Sphere4ClosestFeatures");
	nc->addCollisionFunctor("Sphere","Mesh2D","Sphere2Mesh2D4ClosestFeatures");
	

	rootBody->actors.resize(3);
	rootBody->actors[0] 		= shared_ptr<Actor>(new SAPCollider);
	rootBody->actors[1] 		= nc;
	rootBody->actors[2] 		= shared_ptr<Actor>(new MassSpringBody2RigidBodyDynamicEngine);


	

	rootBody->isDynamic      = false;
	rootBody->velocity       = Vector3(0,0,0);
	rootBody->angularVelocity= Vector3(0,0,0);
	rootBody->se3		 = Se3(Vector3(0,0,0),q);

	//for(int i=0;i<7;i++)
	//	rootBody->kinematic->subscribedBodies.push_back(i);

	shared_ptr<MassSpringBody> cloth(new MassSpringBody);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Mesh2D> mesh2d(new Mesh2D);
	cloth->actors.push_back(shared_ptr<Actor>(new ExplicitMassSpringDynamicEngine));
	cloth->isDynamic	= true;
	cloth->angularVelocity	= Vector3(0,0,0);
	cloth->velocity		= Vector3(0,0,0);
	cloth->mass		= mass;
	cloth->se3		= Se3(Vector3(0,0,0),q);
	cloth->stiffness	= 500;
	cloth->damping		= 0.1;
	for(int i=0;i<width*height;i++)
		cloth->properties.push_back(NodeProperties((float)(width*height)/mass));

	cloth->properties[offset(0,0)].invMass = 0;
	cloth->properties[offset(width-1,0)].invMass = 0;
	//cloth->properties[offset(0,height-1)].invMass = 0;
	//cloth->properties[offset(width-1,height-1)].invMass = 0;

	aabb->color		= Vector3(1,0,0);
	aabb->center		= Vector3(0,0,0);
	aabb->halfSize		= Vector3((cellSize*(width-1))*0.5,0,(cellSize*(height-1))*0.5);
	cloth->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);

	mesh2d->width = width;
	mesh2d->height = height;

	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
			mesh2d->vertices.push_back(Vector3(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5));

	for(int i=0;i<width-1;i++)
		for(int j=0;j<height-1;j++)
		{
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i+1,j)));
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i,j+1)));
			mesh2d->edges.push_back(Edge(offset(i,j+1),offset(i+1,j)));

			vector<int> face1,face2;
			face1.push_back(offset(i,j));
			face1.push_back(offset(i+1,j));
			face1.push_back(offset(i,j+1));

			face2.push_back(offset(i+1,j));
			face2.push_back(offset(i+1,j+1));
			face2.push_back(offset(i,j+1));

			mesh2d->faces.push_back(face1);
			mesh2d->faces.push_back(face2);
		}

	for(int i=0;i<width-1;i++)
		mesh2d->edges.push_back(Edge(offset(i,height-1),offset(i+1,height-1)));

	for(int j=0;j<height-1;j++)
		mesh2d->edges.push_back(Edge(offset(width-1,j),offset(width-1,j+1)));

	for(unsigned int i=0;i<mesh2d->edges.size();i++)
	{
		Vector3 v1 = mesh2d->vertices[mesh2d->edges[i].first];
		Vector3 v2 = mesh2d->vertices[mesh2d->edges[i].second];
		cloth->initialLengths.push_back((v1-v2).length());
	}

	mesh2d->diffuseColor	= Vector3(0,0,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	cloth->cm		= dynamic_pointer_cast<CollisionGeometry>(mesh2d);
	cloth->gm		= dynamic_pointer_cast<CollisionGeometry>(mesh2d);


	rootBody->bodies.push_back(dynamic_pointer_cast<Body>(cloth));


	for(int i=0;i<1/*nbSpheres*/;i++)
	{
		shared_ptr<RigidBody> s(new RigidBody);
		shared_ptr<AABB> aabb(new AABB);
		shared_ptr<Sphere> csphere(new Sphere);
		shared_ptr<Sphere> gsphere(new Sphere);


		Vector3 translation(0,-60,0);
		float radius = 50;

		//Vector3 translation(100*Rand::symmetricRandom(),10+100*Rand::unitRandom(),100*Rand::symmetricRandom());
		//float radius = 0.5*(20+10*Rand::unitRandom());
		//shared_ptr<BallisticDynamicEngine> ballistic(new BallisticDynamicEngine);
		//ballistic->damping 	= 0.999;
		//s->dynamic		= dynamic_pointer_cast<DynamicEngine>(ballistic);

		s->isDynamic		= true;
		s->angularVelocity	= Vector3(0,0,0);
		s->velocity		= Vector3(0,0,0);
		s->mass			= 4.0/3.0*Constants::PI*radius*radius;
		s->inertia		= Vector3(2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius,2.0/5.0*s->mass*radius*radius);
		s->se3			= Se3(translation,q);

		aabb->color		= Vector3(0,1,0);
		aabb->center		= translation;
		aabb->halfSize		= Vector3(radius,radius,radius);
		s->bv			= dynamic_pointer_cast<BoundingVolume>(aabb);

		csphere->radius		= radius*1.2;
		csphere->diffuseColor	= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
		csphere->wire		= false;
		csphere->visible	= true;

		gsphere->radius		= radius;
		gsphere->diffuseColor	= Vector3(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
		gsphere->wire		= false;
		gsphere->visible	= true;

		s->cm			= dynamic_pointer_cast<CollisionGeometry>(csphere);
		s->gm			= dynamic_pointer_cast<GeometricalModel>(gsphere);

		rootBody->bodies.push_back(dynamic_pointer_cast<Body>(s));
	}

// FIXME : why it save twice some attributes?
//	IOManager::loadFromFile("XMLManager", "../data/HangingCloth.xml", "rootBody", rootBody);
	IOManager::saveToFile("XMLManager", "../data/HangingCloth.xml", "rootBody", rootBody);

}
