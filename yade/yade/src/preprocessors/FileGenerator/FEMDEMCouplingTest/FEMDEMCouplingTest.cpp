/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FEMDEMCouplingTest.hpp"

// dispatchers
#include "BoundingVolumeDispatcher.hpp"
#include "BodyPhysicalParametersDispatcher.hpp"
#include "GeometricalModelDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"

// actors
#include "GravityCondition.hpp"
#include "CundallNonViscousForceDampingFunctor.hpp"
#include "CundallNonViscousMomentumDampingFunctor.hpp"
// actors FEM
#include "FEMLaw.hpp"
#include "ActionParameterInitializer.hpp"
#include "ActionParameterReset.hpp"
#include "FEMSetTextLoaderFunctor.hpp"
//actors DEM
#include "SDECTimeStepper.hpp"
#include "ElasticContactLaw.hpp"
#include "PersistentSAPCollider.hpp"

// data
#include "AABB.hpp"
// data FEM
#include "FEMSetParameters.hpp"
#include "InteractionDescriptionSet.hpp"
// data DEM
#include "BodyMacroParameters.hpp"
#include "Box.hpp"
#include "InteractionBox.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FEMDEMCouplingTest::FEMDEMCouplingTest() : FileGenerator()
{
	femTxtFile 		= "../data/fem.beam";
	nodeGroupMask 		= 1;
	tetrahedronGroupMask 	= 2;
	demGroupMask 		= 4;
	
	gravity 		= Vector3r(0,-9.81,0);
	groundSize 		= Vector3r(100,1,100);
	groundPosition 		= Vector3r(0,-8,0);
		
	regionMin1 		= Vector3r(9,-20,-20);
	regionMax1 		= Vector3r(10,20,20);
	radiusFEMDEM1 		= 5.1;
	
	regionMin2 		= Vector3r(-8,-2,6);
	regionMax2 		= Vector3r(-8,0,20);
	radiusFEMDEM2 		= 5.1;

	dampingForce 		= 0.2;
	dampingMomentum 	= 0.2;
	
	spheresOrigin 		= Vector3r(10,10,10);
	nbSpheres 		= Vector3r(5,5,10);
	radiusDEM 		= 5.01;
	density 		= 2.6;
	supportSize 		= 0.5;
	support1 		= true;
	support2 		= true;
	timeStepUpdateInterval 	= 200;
		
	sphereYoungModulus 	= 10000000;
	spherePoissonRatio 	= 0.2;
	sphereFrictionDeg 	= 18.0;
				
	momentRotationLaw  	= true;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FEMDEMCouplingTest::~FEMDEMCouplingTest()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMDEMCouplingTest::registerAttributes()
{
	REGISTER_ATTRIBUTE(femTxtFile);
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(groundSize);
	REGISTER_ATTRIBUTE(groundPosition);
		
	REGISTER_ATTRIBUTE(regionMin1);
	REGISTER_ATTRIBUTE(regionMax1);
	REGISTER_ATTRIBUTE(radiusFEMDEM1);
	
	REGISTER_ATTRIBUTE(regionMin2);
	REGISTER_ATTRIBUTE(regionMax2);
	REGISTER_ATTRIBUTE(radiusFEMDEM2);

	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	
	REGISTER_ATTRIBUTE(spheresOrigin);
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(radiusDEM);
	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(supportSize);
	REGISTER_ATTRIBUTE(support1);
	REGISTER_ATTRIBUTE(support2);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
		
//	REGISTER_ATTRIBUTE(sphereYoungModulus);
//	REGISTER_ATTRIBUTE(spherePoissonRatio);
//	REGISTER_ATTRIBUTE(sphereFrictionDeg);
				
	momentRotationLaw  	= true;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string FEMDEMCouplingTest::generate()
{
	/*
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	positionRootBody(rootBody);
	createActors(rootBody);
	insertGround(rootBody);
	*/
	return "Not ready yet.";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMDEMCouplingTest::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionSphere","AABB","Sphere2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionBox","AABB","Box2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractionSphere","InteractionSphere","Sphere2Sphere4MacroMicroContactGeometry");
	interactionGeometryDispatcher->add("InteractionSphere","InteractionBox","Box2Sphere4MacroMicroContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<FEMSetTextLoaderFunctor> femSetTextLoaderFunctor	= shared_ptr<FEMSetTextLoaderFunctor>(new FEMSetTextLoaderFunctor);
	femSetTextLoaderFunctor->fileName = femTxtFile;

	shared_ptr<BodyPhysicalParametersDispatcher> bodyPhysicalParametersDispatcher(new BodyPhysicalParametersDispatcher);
	bodyPhysicalParametersDispatcher->add("FEMSetParameters","FEMTetrahedronStiffness");
	
	shared_ptr<GeometricalModelDispatcher> geometricalModelDispatcher	= shared_ptr<GeometricalModelDispatcher>(new GeometricalModelDispatcher);
	geometricalModelDispatcher->add("FEMSetParameters","FEMSetGeometry","FEMSet2Tetrahedrons");
	
	shared_ptr<BodyPhysicalParametersDispatcher> positionIntegrator(new BodyPhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegratorFunctor");
	shared_ptr<BodyPhysicalParametersDispatcher> orientationIntegrator(new BodyPhysicalParametersDispatcher);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegratorFunctor");
	
	shared_ptr<FEMLaw> femLaw(new FEMLaw);
	femLaw->nodeGroupMask = nodeGroupMask;
	femLaw->tetrahedronGroupMask = tetrahedronGroupMask;

	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = demGroupMask;
	sdecTimeStepper->interval = timeStepUpdateInterval;
	
	shared_ptr<ElasticContactLaw> demLaw(new ElasticContactLaw);
	demLaw->sdecGroupMask = demGroupMask;
	demLaw->momentRotationLaw = momentRotationLaw;
	
	shared_ptr<CundallNonViscousForceDampingFunctor> actionForceDamping(new CundallNonViscousForceDampingFunctor);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDampingFunctor> actionMomentumDamping(new CundallNonViscousMomentumDampingFunctor);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionParameterDispatcher> actionDampingDispatcher(new ActionParameterDispatcher);
	actionDampingDispatcher->add("ActionParameterForce","ParticleParameters","CundallNonViscousForceDampingFunctor",actionForceDamping);
	actionDampingDispatcher->add("ActionParameterMomentum","RigidBodyParameters","CundallNonViscousMomentumDampingFunctor",actionMomentumDamping);
	
	shared_ptr<GravityCondition> gravityCondition(new GravityCondition);
	gravityCondition->gravity = gravity;
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","ParticleParameters","NewtonsForceLawFunctor");
	applyActionDispatcher->add("ActionParameterMomentum","RigidBodyParameters","NewtonsMomentumLawFunctor");
	
	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum");
	
	rootBody->actors.clear();
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionParameterReset));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new PersistentSAPCollider));
	rootBody->actors.push_back(geometricalModelDispatcher);
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(femLaw);
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	rootBody->actors.push_back(orientationIntegrator);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(bodyPhysicalParametersDispatcher);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
	rootBody->initializers.push_back(actionParameterInitializer);
	
	femSetTextLoaderFunctor->go(rootBody->physicalParameters,rootBody.get()); // load FEM from file.

// will not run - function is protected.
//	rootBody->postProcessAttributes(true); // we don't want to save 'nan' as tetrahedrons' coordinates
// so call is by hand...

	geometricalModelDispatcher->action(rootBody.get() );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMDEMCouplingTest::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic			= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<FEMSetParameters> physics(new FEMSetParameters);
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->nodeGroupMask 			= nodeGroupMask;
	physics->tetrahedronGroupMask 		= tetrahedronGroupMask;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	
	set->diffuseColor			= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm 	= dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("FEMSetGeometry"));
	gm->diffuseColor 			= Vector3r(1,1,1);
	gm->wire 				= false;
	gm->visible 				= true;
	gm->shadowCaster 			= true;
	
	rootBody->interactionGeometry 		= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 		= gm;
	rootBody->physicalParameters 		= physics;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
 
void FEMDEMCouplingTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,demGroupMask));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
 
void FEMDEMCouplingTest::insertGround(shared_ptr<MetaBody>& rootBody) 
{
	shared_ptr<Body> ground;
//	shared_ptr<Body> supportBox1;
//	shared_ptr<Body> supportBox2;
	
	createBox(ground, groundPosition , groundSize );
//	createBox(supportBox1, Vector3r(0,0,((Real)(nbSpheres[2])/2.0-supportSize+1.5)*spacing), Vector3r(20,50,20));
//	createBox(supportBox2, Vector3r(0,0,-((Real)(nbSpheres[2])/2.0-supportSize+2.5)*spacing), Vector3r(20,50,20));
			
	rootBody->bodies->insert(ground);
//	if (support1)
///		rootBody->bodies->insert(supportBox1);
//	if (support2)
//		rootBody->bodies->insert(supportBox2);
		
}
	
