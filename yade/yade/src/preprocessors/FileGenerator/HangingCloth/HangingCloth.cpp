#include "HangingCloth.hpp"


#include "Mesh2D.hpp"
#include "Sphere.hpp"
#include "RigidBody.hpp"
#include "AABB.hpp"
#include "NonConnexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "ParticleBallisticEngine.hpp"
#include "PersistentSAPCollider.hpp"
#include "MassSpringBody.hpp"
#include "ExplicitMassSpringDynamicEngine.hpp"
#include "MassSpringBody2RigidBodyDynamicEngine.hpp"

#include "IOManager.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "ActionDispatcher.hpp"
#include "BoundingVolumeUpdator.hpp"
#include "CollisionGeometrySet2AABBFactory.hpp"
#include "CollisionGeometrySet.hpp"
#include "Particle.hpp"
#include "SpringGeometry.hpp"
#include "SpringPhysics.hpp"

HangingCloth::HangingCloth () : FileGenerator()
{

	width = 20;
	height = 20;
	stiffness = 500;
	springDamping   = 0.1;
	mass = 10;
	cellSize = 20;
	particleDamping = 0.997;
	fixPoint1 = true;
	fixPoint2 = true;
	fixPoint3 = true;
	fixPoint4 = true;

}

HangingCloth::~HangingCloth ()
{

}

void HangingCloth::postProcessAttributes(bool)
{
}

void HangingCloth::registerAttributes()
{	
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(stiffness);
	REGISTER_ATTRIBUTE(springDamping);
	REGISTER_ATTRIBUTE(mass);
	REGISTER_ATTRIBUTE(particleDamping);
	REGISTER_ATTRIBUTE(cellSize);
	REGISTER_ATTRIBUTE(fixPoint1);
	REGISTER_ATTRIBUTE(fixPoint2);
	REGISTER_ATTRIBUTE(fixPoint3);
	REGISTER_ATTRIBUTE(fixPoint4);
}

string HangingCloth::generate()
{
	
	rootBody = shared_ptr<NonConnexBody>(new MassSpringBody);
	
	Quaternionr q;
	q.fromAxisAngle(Vector3r(0,0,1),0);
 
	//shared_ptr<InteractionGeometryDispatcher> igd(new InteractionGeometryDispatcher);
	//igd->addInteractionGeometryFunctor("Sphere","Sphere","Sphere2Sphere4SDECContactModel");
	
	//shared_ptr<InteractionPhysicsDispatcher> ipd(new InteractionPhysicsDispatcher);
	//ipd->addInteractionPhysicsFunctor("Particle","Particle","SDECLinearContactModel");
		
	shared_ptr<BoundingVolumeUpdator> bvu	= shared_ptr<BoundingVolumeUpdator>(new BoundingVolumeUpdator);
	bvu->addBVFactories("Sphere","AABB","Sphere2AABBFactory");
	bvu->addBVFactories("CollisionGeometrySet","AABB","CollisionGeometrySet2AABBFactory");
	
	shared_ptr<ActionDispatcher> ad(new ActionDispatcher);
	ad->addActionFunctor("ActionForce","Particle","ActionForce2Particle");

	
	rootBody->actors.resize(4);
	rootBody->actors[0] 		= bvu;	
	rootBody->actors[1] 		= shared_ptr<Actor>(new PersistentSAPCollider);
	//rootBody->actors[2] 		= igd;
	//rootBody->actors[3] 		= ipd;
	rootBody->actors[2] 		= shared_ptr<Actor>(new ExplicitMassSpringDynamicEngine);
	rootBody->actors[3] 		= ad;

	rootBody->permanentInteractions->clear();

	rootBody->isDynamic		= false;
	rootBody->velocity		= Vector3r(0,0,0);
	rootBody->angularVelocity	= Vector3r(0,0,0);
	rootBody->se3			= Se3r(Vector3r(0,0,0),q);

	shared_ptr<CollisionGeometrySet> set(new CollisionGeometrySet());
	set->diffuseColor	= Vector3f(0,0,1);
	set->wire		= false;
	set->visible		= true;
	set->shadowCaster	= false;
	rootBody->cm		= dynamic_pointer_cast<CollisionGeometry>(set);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->color		= Vector3r(0,0,1);
	rootBody->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
	
	shared_ptr<Mesh2D> mesh2d(new Mesh2D);
	mesh2d->diffuseColor	= Vector3f(0,0,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	mesh2d->shadowCaster	= false;
	
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
		{
			shared_ptr<Particle> particle(new Particle);
			particle->position 		= Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5);
			particle->isDynamic		= true;
			particle->angularVelocity	= Vector3r(0,0,0);
			particle->velocity		= Vector3r(0,0,0);
			particle->mass			= mass/(Real)(width*height);
			particle->se3			= Se3r(particle->position,q);
			shared_ptr<AABB> aabb(new AABB);
			aabb->color		= Vector3r(0,1,0);
			particle->bv		= dynamic_pointer_cast<BoundingVolume>(aabb);
			shared_ptr<Sphere> sphere(new Sphere);
			sphere->diffuseColor	= Vector3r(0,0,1);
			sphere->radius		= cellSize/2.0;
			sphere->wire		= false;
			sphere->visible		= true;
			sphere->shadowCaster		= false;
			particle->cm		= dynamic_pointer_cast<CollisionGeometry>(sphere);
			particle->gm		= particle->cm;
			
			shared_ptr<ParticleBallisticEngine> ballistic(new ParticleBallisticEngine);
			ballistic->damping 	= particleDamping;
			particle->actors.push_back(ballistic);
			
			shared_ptr<Body> body = dynamic_pointer_cast<Body>(particle);
			rootBody->bodies->insert(body);
			mesh2d->vertices.push_back(particle->position);
		}
		
	for(int i=0;i<width-1;i++)
		for(int j=0;j<height-1;j++)
		{
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i+1,j)));
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i,j+1)));
			mesh2d->edges.push_back(Edge(offset(i,j+1),offset(i+1,j)));
			
			rootBody->permanentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i+1,j)));
			rootBody->permanentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i,j+1)));
			rootBody->permanentInteractions->insert(createSpring(rootBody,offset(i,j+1),offset(i+1,j)));
			
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
	{
		mesh2d->edges.push_back(Edge(offset(i,height-1),offset(i+1,height-1)));
		rootBody->permanentInteractions->insert(createSpring(rootBody,offset(i,height-1),offset(i+1,height-1)));
		
	}
	
	for(int j=0;j<height-1;j++)
	{
		mesh2d->edges.push_back(Edge(offset(width-1,j),offset(width-1,j+1)));
		rootBody->permanentInteractions->insert(createSpring(rootBody,offset(width-1,j),offset(width-1,j+1)));
	}
	
	rootBody->gm = mesh2d;

	if (fixPoint1)
	{
		Particle * p = static_cast<Particle*>((*(rootBody->bodies))[offset(0,0)].get());
		p->invMass = 0;
		p->cm->diffuseColor = Vector3r(1.0,0.0,0.0);
		p->actors.clear();
	}
	
	if (fixPoint2)
	{
		Particle * p = static_cast<Particle*>((*(rootBody->bodies))[offset(width-1,0)].get());
		p->invMass = 0;
		p->cm->diffuseColor = Vector3r(1.0,0.0,0.0);
		p->actors.clear();
	}
	
	if (fixPoint3)
	{
		Particle * p = static_cast<Particle*>((*(rootBody->bodies))[offset(0,height-1)].get());
		p->invMass = 0;
		p->cm->diffuseColor = Vector3r(1.0,0.0,0.0);		
		p->actors.clear();
	}

	if (fixPoint4)
	{
		Particle * p = static_cast<Particle*>((*(rootBody->bodies))[offset(width-1,height-1)].get());
		p->invMass = 0;
		p->cm->diffuseColor = Vector3r(1.0,0.0,0.0);		
		p->actors.clear();
	}
 	
	return "";
}


shared_ptr<Interaction>& HangingCloth::createSpring(const shared_ptr<NonConnexBody>& rootBody,int i,int j)
{
	
	Particle * p1 = static_cast<Particle*>((*(rootBody->bodies))[i].get());
	Particle * p2 = static_cast<Particle*>((*(rootBody->bodies))[j].get());
	
	spring = shared_ptr<Interaction>(new Interaction( p1->getId() , p2->getId() ));
	shared_ptr<SpringGeometry>	geometry(new SpringGeometry);
	shared_ptr<SpringPhysics>	physics(new SpringPhysics);
	
	geometry->p1			= p1->position;
	geometry->p2			= p2->position;
	
	physics->initialLength		= (geometry->p1-geometry->p2).length();
	physics->stiffness		= stiffness;
	physics->damping		= springDamping;
	
	spring->interactionGeometry = geometry;
	spring->interactionPhysics = physics;
	spring->isReal = true;
	spring->isNew = false;
	
	return spring;
}
