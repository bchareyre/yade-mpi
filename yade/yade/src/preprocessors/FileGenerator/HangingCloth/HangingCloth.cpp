#include "HangingCloth.hpp"

#include "SimpleBody.hpp"
#include "Mesh2D.hpp"
#include "Sphere.hpp"
#include "RigidBodyParameters.hpp"
#include "AABB.hpp"
#include "ComplexBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "PersistentSAPCollider.hpp"
#include "ComplexBody.hpp"
#include "ExplicitMassSpringDynamicEngine.hpp"
#include "MassSpringBody2RigidBodyDynamicEngine.hpp"

#include "IOManager.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "ApplyActionDispatcher.hpp"
#include "BoundingVolumeDispatcher.hpp"
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "InteractionDescriptionSet.hpp"
#include "ParticleParameters.hpp"
#include "SpringGeometry.hpp"
#include "SpringPhysics.hpp"
#include "TimeIntegratorDispatcher.hpp"
#include "InteractionSphere.hpp"

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
	
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<InteractionGeometryDispatcher> igd(new InteractionGeometryDispatcher);
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionSphere","Sphere2Sphere4SDECContactModel");
	igd->addInteractionGeometryFunctor("InteractionSphere","InteractionBox","Box2Sphere4SDECContactModel");

	shared_ptr<InteractionPhysicsDispatcher> ipd(new InteractionPhysicsDispatcher);
	ipd->addInteractionPhysicsFunctor("SDECParameters","SDECParameters","SDECLinearContactModel");
		
	shared_ptr<BoundingVolumeDispatcher> bvu	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	bvu->addBoundingVolumeFunctors("InteractionSphere","AABB","Sphere2AABBFunctor");
	bvu->addBoundingVolumeFunctors("InteractionBox","AABB","Box2AABBFunctor");
	bvu->addBoundingVolumeFunctors("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");
	
	shared_ptr<ApplyActionDispatcher> ad(new ApplyActionDispatcher);
	ad->addApplyActionFunctor("ActionForce","ParticleParameters","ActionForce2Particle");
	
	shared_ptr<TimeIntegratorDispatcher> ti(new TimeIntegratorDispatcher);
	ti->addTimeIntegratorFunctor("ParticleParameters","LeapFrogIntegrator");
	
	rootBody->actors.resize(5);
	rootBody->actors[0] 		= bvu;	
	rootBody->actors[1] 		= shared_ptr<Actor>(new PersistentSAPCollider);
	//rootBody->actors[2] 		= igd;
	//rootBody->actors[3] 		= ipd;
	rootBody->actors[2] 		= shared_ptr<Actor>(new ExplicitMassSpringDynamicEngine);
	rootBody->actors[3] 		= ad;
	rootBody->actors[4] 		= ti;
	

	//FIXME : use a default one
	shared_ptr<ParticleParameters> physics2(new ParticleParameters); // FIXME : fix indexable class BodyPhysicalParameters
	physics2->se3		= Se3r(Vector3r(0,0,0),q);
	physics2->mass		= 0;
	physics2->velocity	= Vector3r::ZERO;
	
	rootBody->isDynamic	= false;

	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	set->diffuseColor	= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor	= Vector3r(0,0,1);
	
	shared_ptr<Mesh2D> mesh2d(new Mesh2D);
	mesh2d->diffuseColor	= Vector3f(0,0,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	mesh2d->shadowCaster	= false;
	
	rootBody->geometricalModel			= mesh2d;
	rootBody->interactionGeometry			= set;
	rootBody->boundingVolume			= aabb;
	rootBody->physicalParameters	= physics2;
	
	rootBody->permanentInteractions->clear();
	
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
		{
			shared_ptr<Body> node(new SimpleBody);
			
			node->isDynamic		= true;
			
			shared_ptr<ParticleParameters> particle(new ParticleParameters);
			particle->velocity		= Vector3r(0,0,0);
			particle->mass			= mass/(Real)(width*height);
			particle->se3			= Se3r(Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5),q);
			
			shared_ptr<AABB> aabb(new AABB);
			aabb->diffuseColor	= Vector3r(0,1,0);
			
			shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
			iSphere->diffuseColor	= Vector3r(0,0,1);
			iSphere->radius		= cellSize/2.0;

			node->boundingVolume		= aabb;
			//node->geometricalModel		= ??;
			node->interactionGeometry		= iSphere;
			node->physicalParameters= particle;
			
			rootBody->bodies->insert(node);
			mesh2d->vertices.push_back(particle->se3.translation);
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

	if (fixPoint1)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}
	
	if (fixPoint2)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);
		body->isDynamic = false;
	}
	
	if (fixPoint3)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);		
		body->isDynamic = false;
	}

	if (fixPoint4)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3r(1.0,0.0,0.0);		
		body->isDynamic = false;
	}
 	
	return "";
}


shared_ptr<Interaction>& HangingCloth::createSpring(const shared_ptr<ComplexBody>& rootBody,int i,int j)
{
	SimpleBody * b1 = static_cast<SimpleBody*>((*(rootBody->bodies))[i].get());
	SimpleBody * b2 = static_cast<SimpleBody*>((*(rootBody->bodies))[j].get());
	
	spring = shared_ptr<Interaction>(new Interaction( b1->getId() , b2->getId() ));
	shared_ptr<SpringGeometry>	geometry(new SpringGeometry);
	shared_ptr<SpringPhysics>	physics(new SpringPhysics);
	
	geometry->p1			= b1->physicalParameters->se3.translation;
	geometry->p2			= b2->physicalParameters->se3.translation;
	
	physics->initialLength		= (geometry->p1-geometry->p2).length();
	physics->stiffness		= stiffness;
	physics->damping		= springDamping;
	
	spring->interactionGeometry = geometry;
	spring->interactionPhysics = physics;
	spring->isReal = true;
	spring->isNew = false;
	
	return spring;
}
