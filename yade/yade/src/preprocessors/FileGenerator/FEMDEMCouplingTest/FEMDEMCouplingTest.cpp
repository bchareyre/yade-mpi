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

// actors
#include "FEMLaw.hpp"
#include "ActionParameterInitializer.hpp"
#include "ActionParameterReset.hpp"
#include "FEMSetTextLoaderFunctor.hpp"
#include "GravityCondition.hpp"

// data
#include "AABB.hpp"
#include "FEMSetParameters.hpp"
#include "InteractionDescriptionSet.hpp"


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FEMDEMCouplingTest::FEMDEMCouplingTest() : FileGenerator()
{
	femTxtFile 		= "../data/fem.beam";
	nodeGroupMask 		= 1;
	tetrahedronGroupMask 	= 2;
	demGroupMask 		= 4;
	gravity 		= Vector3r(0,-9.81,0);
		
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
		
	sphereYoungModulus 	=   10000000;
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
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
	positionRootBody(rootBody);
	createActors(rootBody);

	return "Not ready yet.";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMDEMCouplingTest::createActors(shared_ptr<ComplexBody>& rootBody)
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

	shared_ptr<FEMSetTextLoaderFunctor> femSetTextLoaderFunctor	= shared_ptr<FEMSetTextLoaderFunctor>(new FEMSetTextLoaderFunctor);
	femSetTextLoaderFunctor->fileName = femTxtFile;

	shared_ptr<BodyPhysicalParametersDispatcher> bodyPhysicalParametersDispatcher(new BodyPhysicalParametersDispatcher);
	bodyPhysicalParametersDispatcher->add("FEMSetParameters","FEMTetrahedronStiffness");
	
	shared_ptr<GeometricalModelDispatcher> geometricalModelDispatcher	= shared_ptr<GeometricalModelDispatcher>(new GeometricalModelDispatcher);
	geometricalModelDispatcher->add("FEMSetParameters","FEMSetGeometry","FEMSet2Tetrahedrons");
	
	shared_ptr<BodyPhysicalParametersDispatcher> positionIntegrator(new BodyPhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegratorFunctor");
	
	shared_ptr<FEMLaw> femLaw(new FEMLaw);
	femLaw->nodeGroupMask = nodeGroupMask;
	femLaw->tetrahedronGroupMask = tetrahedronGroupMask;

	shared_ptr<GravityCondition> gravityCondition(new GravityCondition);
	gravityCondition->gravity = gravity;
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","ParticleParameters","NewtonsForceLawFunctor");
	
	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum"); // FIXME - should be unnecessery, but BUG in ActionParameterVectorVector
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionParameterReset));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(geometricalModelDispatcher);
	rootBody->actors.push_back(femLaw);
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	
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

void FEMDEMCouplingTest::positionRootBody(shared_ptr<ComplexBody>& rootBody) 
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
	
	rootBody->interactionGeometry 		= dynamic_pointer_cast<InteractionDescription>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 		= gm;
	rootBody->physicalParameters 		= physics;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
 
