/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  with help from Emmanuel Frangin                                       *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMBeam.hpp"

#include<yade/pkg-fem/FEMSetParameters.hpp>
#include<yade/pkg-fem/FEMTetrahedronData.hpp>
#include<yade/pkg-fem/FEMNodeData.hpp>
#include<yade/pkg-fem/FEMTetrahedronStiffness.hpp>
#include<yade/pkg-fem/FEMLaw.hpp>
#include<yade/pkg-fem/FEMSetTextLoader.hpp>


// data
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Tetrahedron.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

// engines
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-common/TranslationEngine.hpp>

// body
#include<yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>

// dispatchers
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>

#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/GeometricalModelMetaEngine.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>


#include <boost/filesystem/convenience.hpp>



using namespace boost;
using namespace std;


FEMBeam::FEMBeam () : FileGenerator()
{
	gravity 		= Vector3r(0,-9.81,0);
	femTxtFile 		= "./fem.beam";
	nodeGroupMask 		= 1;
	tetrahedronGroupMask 	= 2;

	regionMin1 		= Vector3r(9,-20,-20);
	regionMax1 		= Vector3r(10,20,20);
	translationAxis1 	= Vector3r(1,0,0);
	velocity1 		= 0;
	
	regionMin2 		= Vector3r(-8,-2,6);
	regionMax2 		= Vector3r(-8,0,20);
	translationAxis2 	= Vector3r(-1,0,0); 
	velocity2 		= 0.0;

/*	
	regionMin1 		= Vector3r(9,0,-20);
	regionMax1 		= Vector3r(10,20,20);
	translationAxis1 	= Vector3r(1,0,0);
	velocity1 		= 0.5;
	
	regionMin2 		= Vector3r(-11,-2,6);
	regionMax2 		= Vector3r(-8,0,20);
	translationAxis2 	= Vector3r(-1,0,0);
	velocity2 		= 0.0;
*/		
}


FEMBeam::~FEMBeam ()
{ 
}


void FEMBeam::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(femTxtFile);
	REGISTER_ATTRIBUTE(gravity);
	
	REGISTER_ATTRIBUTE(regionMin1);
	REGISTER_ATTRIBUTE(regionMax1);
	REGISTER_ATTRIBUTE(translationAxis1);
	REGISTER_ATTRIBUTE(velocity1);
	
	REGISTER_ATTRIBUTE(regionMin2);
	REGISTER_ATTRIBUTE(regionMax2);
	REGISTER_ATTRIBUTE(translationAxis2);
	REGISTER_ATTRIBUTE(velocity2);
}


bool FEMBeam::generate()
{
	Omega::instance().setTimeStep(0.004);
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	positionRootBody(rootBody);
	
	
	rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);
	
	
	
	createActors(rootBody);
	imposeTranslation(rootBody,regionMin1,regionMax1,translationAxis1,velocity1);
	imposeTranslation(rootBody,regionMin2,regionMax2,translationAxis2,velocity2);

	#ifdef NDEBUG
		message="FEM currently works only in not optimized debug mode and needs to be fixed. If you want to try it you must compile with option 'scons optimize=0'";
		return false;
	#endif

	if(filesystem::exists(femTxtFile))
	{
		message="NOTE: if it explodes, set smaller time step."; 
		return true;
	}
	else
	{
		message="Cannot find input file, you can copy it from examples/ directory"; 
		return false;
	}
}


void FEMBeam::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");

	shared_ptr<FEMSetTextLoader> femSetTextLoaderFunctor	= shared_ptr<FEMSetTextLoader>(new FEMSetTextLoader);
	femSetTextLoaderFunctor->fileName = femTxtFile;

	shared_ptr<PhysicalParametersMetaEngine> bodyPhysicalParametersDispatcher(new PhysicalParametersMetaEngine);
	bodyPhysicalParametersDispatcher->add("FEMTetrahedronStiffness");
	
	shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher	= shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
	geometricalModelDispatcher->add("FEMSet2Tetrahedrons");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	
	shared_ptr<FEMLaw> femLaw(new FEMLaw);
	femLaw->nodeGroupMask = nodeGroupMask;
	femLaw->tetrahedronGroupMask = tetrahedronGroupMask;

	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	
	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum"); // FIXME - should be unnecessery, but BUG in PhysicalActionVectorVector
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(geometricalModelDispatcher);
	rootBody->engines.push_back(femLaw);
	rootBody->engines.push_back(gravityCondition);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(bodyPhysicalParametersDispatcher);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
	rootBody->initializers.push_back(physicalActionInitializer);
	
	femSetTextLoaderFunctor->go(rootBody->physicalParameters,rootBody.get()); // load FEM from file.

// will not run - function is protected.
//	rootBody->postProcessAttributes(true); // we don't want to save 'nan' as tetrahedrons' coordinates
// so call is by hand...

	geometricalModelDispatcher->action(rootBody.get() );

}


void FEMBeam::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic			= false;

	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<FEMSetParameters> physics(new FEMSetParameters);
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->nodeGroupMask 			= nodeGroupMask;
	physics->tetrahedronGroupMask 		= tetrahedronGroupMask;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor			= Vector3r(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm 	= YADE_PTR_CAST<GeometricalModel>(ClassFactory::instance().createShared("FEMSetGeometry"));
	gm->diffuseColor 			= Vector3r(1,1,1);
	gm->wire 				= false;
	gm->visible 				= true;
	gm->shadowCaster 			= true;
	
	rootBody->interactingGeometry 		= YADE_PTR_CAST<InteractingGeometry>(set);	
	rootBody->boundingVolume		= YADE_PTR_CAST<BoundingVolume>(aabb);
	rootBody->geometricalModel 		= gm;
	rootBody->physicalParameters 		= physics;
}

 
void FEMBeam::imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity)
{
	shared_ptr<TranslationEngine> translationCondition = shared_ptr<TranslationEngine>(new TranslationEngine);
 	translationCondition->velocity  = velocity;
	direction.Normalize();
 	translationCondition->translationAxis = direction;
	
	rootBody->engines.push_back(translationCondition);
	translationCondition->subscribedBodies.clear();
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if( b->getGroupMask() & nodeGroupMask )
		{
			Vector3r pos = b->physicalParameters->se3.position;
			if(        pos[0] > min[0] 
				&& pos[1] > min[1] 
				&& pos[2] > min[2] 
				&& pos[0] < max[0] 
				&& pos[1] < max[1] 
				&& pos[2] < max[2] )
			{
				b->isDynamic = false;
				b->geometricalModel->diffuseColor = Vector3r(1,0,0);
				translationCondition->subscribedBodies.push_back(b->getId());
			}
		}
	}
}

 
YADE_PLUGIN();
