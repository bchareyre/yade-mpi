/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "SDECSpheresPlane.hpp"

#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>


#include<yade/pkg-common/BoxModel.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/SphereModel.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundDispatcher.hpp>
#include<yade/pkg-common/SceneShape.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>

#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>

#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/GravityEngines.hpp>

#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/StateMetaEngine.hpp>

YADE_REQUIRE_FEATURE(geometricalmodel);

SDECSpheresPlane::SDECSpheresPlane () : FileGenerator()
{
	nbSpheres = Vector3r(2,3,2);
	minRadius = 5;
	maxRadius = 5;
	groundSize = Vector3r(200,5,200);
	dampingForce = 0.3;
	dampingMomentum = 0.3;
	timeStepUpdateInterval = 300;
	sphereYoungModulus   = 15000000.0;
	//sphereYoungModulus   = 10000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density = 2600;
	rotationBlocked = false;
	gravity = Vector3r(0,-9.81,0);
	disorder = Vector3r(0.2,0.2,0.2);
	useSpheresAsGround = false;
	spheresHeight = 0;
}


SDECSpheresPlane::~SDECSpheresPlane ()
{

}


void SDECSpheresPlane::postProcessAttributes(bool)
{

}



bool SDECSpheresPlane::generate()
{
	rootBody = shared_ptr<Scene>(new Scene);
	createActors(rootBody);
	positionRootBody(rootBody);

		
////////////////////////////////////
///////// ground

	if (!useSpheresAsGround)
	{
		shared_ptr<Body> ground;
		createBox(ground, Vector3r(0,0,0), groundSize);
		rootBody->bodies->insert(ground);
	}
	else
	{
		int nbSpheresi,nbSpheresj,nbSpheresk;
		Real radius = groundSize[0];

		if (groundSize[1]<radius)
			radius = groundSize[1];
		if (groundSize[2]<radius)
			radius = groundSize[2];

		nbSpheresi = (int)(groundSize[0]/radius);
		nbSpheresj = (int)(groundSize[1]/radius);
		nbSpheresk = (int)(groundSize[2]/radius);

		for(int i=0;i<nbSpheresi;i++)
		{
			if(shouldTerminate()) return "";

			for(int j=0;j<nbSpheresj;j++)
				for(int k=0;k<nbSpheresk;k++)
				{
					shared_ptr<Body> sphere;
					createGroundSphere(sphere,radius,radius*(i-nbSpheresi*0.5+0.5),radius*(j-nbSpheresj*0.5+0.5),radius*(k-nbSpheresk*0.5+0.5));
					rootBody->bodies->insert(sphere);
				}
		}
	}

///////// spheres
	float all = nbSpheres[0]*nbSpheres[1]*nbSpheres[2];
	float current = 0.0;

	for(int i=0;i<nbSpheres[0];i++)
	{
		if(shouldTerminate()) return "";

		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);

				setProgress(current++/all);
			}
	}
	
	message=""; return true;
}


void SDECSpheresPlane::createGroundSphere(shared_ptr<Body>& body,Real radius, Real i, Real j, Real k)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<SphereModel> gSphere(new SphereModel);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r position		= Vector3r(i,j,k);
	
	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(0.7,0.7,0.7);
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->shape	= iSphere;
	body->geometricalModel		= gSphere;
	body->bound		= aabb;
	body->physicalParameters	= physics;
}


void SDECSpheresPlane::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<SphereModel> gSphere(new SphereModel);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r position		= Vector3r(i,j+spheresHeight,k)*(2*maxRadius*1.1) // this formula is crazy !!
					  - Vector3r( nbSpheres[0]/2*(2*maxRadius*1.1) , -7-maxRadius*2 , nbSpheres[2]/2*(2*maxRadius*1.1) )
					  + Vector3r( 	 Mathr::SymmetricRandom()*disorder[0]
					  		,Mathr::SymmetricRandom()*disorder[1]
							,Mathr::SymmetricRandom()*disorder[2])*maxRadius;
	
	Real radius 			= (Mathr::IntervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->shape	= iSphere;
	body->geometricalModel		= gSphere;
	body->bound		= aabb;
	body->physicalParameters	= physics;
}


void SDECSpheresPlane::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<BoxModel> gBox(new BoxModel);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	//physics->mass			= 0;
	//physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3r(1,1,1);
	gBox->wire			= false;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,1,1);

	body->bound		= aabb;
	body->shape	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}


void SDECSpheresPlane::createActors(shared_ptr<Scene>& rootBody)
{
	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("Ig2_Sphere_Sphere_ScGeom");
	interactionGeometryDispatcher->add("Ig2_Box_Sphere_ScGeom");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("MacroMicroElasticRelationships");
		
	shared_ptr<BoundDispatcher> boundDispatcher	= shared_ptr<BoundDispatcher>(new BoundDispatcher);
	boundDispatcher->add("InteractingSphere2AABB");
	boundDispatcher->add("InteractingBox2AABB");
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add(actionForceDamping);
	actionDampingDispatcher->add(actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	applyActionDispatcher->add("NewtonsMomentumLaw");
	
	shared_ptr<StateMetaEngine> positionIntegrator(new StateMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	shared_ptr<StateMetaEngine> orientationIntegrator(new StateMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");
 	

	shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	sdecTimeStepper->sdecGroupMask = 1;
	sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;

	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(sdecTimeStepper);
	rootBody->engines.push_back(boundDispatcher);	
	rootBody->engines.push_back(shared_ptr<Engine>(new InsertionSortCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new ElasticContactLaw));
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);

	if(!rotationBlocked)
		rootBody->engines.push_back(orientationIntegrator);

	shared_ptr<PositionOrientationRecorder> positionOrientationRecorder(new PositionOrientationRecorder);
	//rootBody->engines.push_back(positionOrientationRecorder);

	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundDispatcher);
}


void SDECSpheresPlane::positionRootBody(shared_ptr<Scene>& rootBody) 
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r(0,0,0);
	physics->acceleration			= Vector3r::ZERO;
		
	shared_ptr<SceneShape> set(new SceneShape());
	set->diffuseColor			= Vector3r(0,0,1);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->shape		= YADE_PTR_CAST<Shape>(set);	
	rootBody->bound		= YADE_PTR_CAST<Bound>(aabb);
	rootBody->physicalParameters 		= physics;
}


YADE_PLUGIN((SDECSpheresPlane));

YADE_REQUIRE_FEATURE(PHYSPAR);

