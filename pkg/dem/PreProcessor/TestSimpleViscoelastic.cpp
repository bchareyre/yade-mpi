/*************************************************************************
*  Copyright (C) 2006 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "TestSimpleViscoelastic.hpp"
#include <yade/extra/Shop.hpp>
#include <yade/core/Body.hpp>
#include <yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/ConstitutiveLawDispatcher.hpp>
#include<yade/pkg-dem/RigidBodyRecorder.hpp>
#include<yade/pkg-dem/SimpleViscoelasticSpheresInteractionRecorder.hpp>
#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>
#include<yade/pkg-dem/ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw.hpp>
#include<yade/pkg-dem/SimpleViscoelasticRelationships.hpp>
#include<yade/pkg-common/GravityEngines.hpp>

TestSimpleViscoelastic::TestSimpleViscoelastic() : FileGenerator()
{
    tc = 0.001;
    en = 0.3;
    es = 0.3;
    frictionAngle = 0.52; // 30 grad
    R  = 1;
    density = 2600;
    nbSpheres = 10;
    h  = 1;
    velocity = Vector3r(0,-1,0);
    angular_vel = Vector3r(0,0,0);
    rotationBlocked=false;
    groundSize = Vector3r(10,1,10);
    gravity = Vector3r(0,-9.81,0);
    outputBase="test";
    interval=50;
}

TestSimpleViscoelastic::~TestSimpleViscoelastic()
{}

void TestSimpleViscoelastic::postProcessAttributes(bool)
{}

void TestSimpleViscoelastic::registerAttributes()
{
    FileGenerator::registerAttributes();
    REGISTER_ATTRIBUTE(tc);
    REGISTER_ATTRIBUTE(en);
    REGISTER_ATTRIBUTE(es);
    REGISTER_ATTRIBUTE(frictionAngle);
    REGISTER_ATTRIBUTE(R);
    REGISTER_ATTRIBUTE(density);
    REGISTER_ATTRIBUTE(nbSpheres);
    REGISTER_ATTRIBUTE(h);
    REGISTER_ATTRIBUTE(velocity);
    REGISTER_ATTRIBUTE(angular_vel);
    REGISTER_ATTRIBUTE(rotationBlocked);
    REGISTER_ATTRIBUTE(gravity);
    REGISTER_ATTRIBUTE(groundSize);
    REGISTER_ATTRIBUTE(outputBase);
    REGISTER_ATTRIBUTE(interval);
}

bool TestSimpleViscoelastic::generate()
{
    rootBody = shared_ptr<MetaBody>(new MetaBody);
    createActors(rootBody);
    positionRootBody(rootBody);
    
////////////////////////////////////
///////// Container
	
    rootBody->transientInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
    rootBody->bodies			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

////////////////////////////////////
///////// ground
    
    shared_ptr<Body> ground;
    createBox(ground, Vector3r(0,0,0),groundSize);
    rootBody->bodies->insert(ground);

////////////////////////////////////
///////// Spheres

    float all = nbSpheres;
    float current = 0.0;
    for(int i=0; i<nbSpheres; ++i)
    {
	if (shouldTerminate()) return false;
	shared_ptr<Body> sphere;
	createSphere(sphere,i);
	rootBody->bodies->insert(sphere);

	setProgress(current++/all);
    }
    return true;
}

void TestSimpleViscoelastic::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<SimpleViscoelasticBodyParameters> physics(new SimpleViscoelasticBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*8; 
	physics->inertia		= Vector3r(physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
						, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
						, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3);
	physics->se3			= Se3r(position,q);
    Shop::getViscoelasticFromSpheresInteraction(physics->mass,tc,en,es,physics);
	physics->frictionAngle			= frictionAngle;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3r(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,1,1);

	body->boundingVolume		= aabb;
	body->interactingGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

void TestSimpleViscoelastic::createActors(shared_ptr<MetaBody>& rootBody)
{
    
    shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
    interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometry");
    interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");

    shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
    interactionPhysicsDispatcher->add("SimpleViscoelasticRelationships");
	    
    shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
    boundingVolumeDispatcher->add("InteractingSphere2AABB");
    boundingVolumeDispatcher->add("InteractingBox2AABB");
    boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");
    
    shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
    gravityCondition->gravity = gravity;
    
    shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
    applyActionDispatcher->add("NewtonsForceLaw");
    applyActionDispatcher->add("NewtonsMomentumLaw");
    
    shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
    positionIntegrator->add("LeapFrogPositionIntegrator");
    shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
    orientationIntegrator->add("LeapFrogOrientationIntegrator");
    
	shared_ptr<ConstitutiveLawDispatcher> constitutiveLaw(new ConstitutiveLawDispatcher);
	constitutiveLaw->add("ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw");

    rootBody->engines.clear();
    rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
    rootBody->engines.push_back(boundingVolumeDispatcher);	
    rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
    rootBody->engines.push_back(interactionGeometryDispatcher);
    rootBody->engines.push_back(interactionPhysicsDispatcher);
    rootBody->engines.push_back(constitutiveLaw);
    rootBody->engines.push_back(gravityCondition);
    rootBody->engines.push_back(applyActionDispatcher);
    rootBody->engines.push_back(positionIntegrator);
    if(!rotationBlocked) rootBody->engines.push_back(orientationIntegrator);

    shared_ptr<RigidBodyRecorder> rigidBodyRecorder(new RigidBodyRecorder);
    rigidBodyRecorder->outputBase = outputBase+string("-body");
    rigidBodyRecorder->interval = interval;
    rootBody->engines.push_back(rigidBodyRecorder);

    shared_ptr<SimpleViscoelasticSpheresInteractionRecorder> interactionRecorder(new SimpleViscoelasticSpheresInteractionRecorder);
    interactionRecorder->outputBase = outputBase+string("-interaction");
    interactionRecorder->interval = interval;
    rootBody->engines.push_back(interactionRecorder);

    rootBody->initializers.clear();
    rootBody->initializers.push_back(boundingVolumeDispatcher);
}

void TestSimpleViscoelastic::createSphere(shared_ptr<Body>& body, int i)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<SimpleViscoelasticBodyParameters> physics(new SimpleViscoelasticBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r position = Vector3r(0,i*(2.0*R+h)+2*groundSize[1]+h,0);
	
//	Real radius 			= (Mathr::IntervalRandom(minRadius,maxRadius));
	Real radius = R;
	
	body->isDynamic	= true;
	
	physics->angularVelocity	= angular_vel;
	physics->velocity		= velocity;
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
    Shop::getViscoelasticFromSpheresInteraction(physics->mass,tc,en,es,physics);
    physics->frictionAngle		= frictionAngle;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}
    

void TestSimpleViscoelastic::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<ParticleParameters> physics(new ParticleParameters); 
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r(0,0,0);
	physics->acceleration			= Vector3r::ZERO;
		
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3r(0,0,1);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactingGeometry		= YADE_PTR_CAST<InteractingGeometry>(set);	
	rootBody->boundingVolume		= YADE_PTR_CAST<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}

    
    
