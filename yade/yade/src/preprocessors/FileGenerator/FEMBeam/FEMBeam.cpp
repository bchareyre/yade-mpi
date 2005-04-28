#include "FEMBeam.hpp"


// data
#include "AABB.hpp"
#include "Sphere.hpp"
#include "Tetrahedron.hpp"
#include "FEMSetParameters.hpp"
#include "FEMTetrahedronData.hpp"
#include "FEMNodeData.hpp"
#include "MetaInteractingGeometry.hpp"

// actors
#include "FEMTetrahedronStiffness.hpp"
#include "CundallNonViscousMomentumDampingFunctor.hpp"
#include "CundallNonViscousForceDampingFunctor.hpp"
#include "ActionParameterInitializer.hpp"
#include "ActionParameterReset.hpp"
#include "FEMLaw.hpp"
#include "FEMSetTextLoaderFunctor.hpp"
#include "GravityCondition.hpp"
#include "TranslationCondition.hpp"

// body
#include "MetaBody.hpp"
#include "Body.hpp"

// dispatchers
#include "PhysicalParametersDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "BoundingVolumeMetaEngine.hpp"
#include "GeometricalModelDispatcher.hpp"

#include <boost/filesystem/convenience.hpp>

using namespace boost;
using namespace std;

FEMBeam::FEMBeam () : FileGenerator()
{
	gravity 		= Vector3r(0,-9.81,0);
	femTxtFile 		= "../data/fem.beam";
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

string FEMBeam::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	positionRootBody(rootBody);
	createActors(rootBody);
	imposeTranslation(rootBody,regionMin1,regionMax1,translationAxis1,velocity1);
	imposeTranslation(rootBody,regionMin2,regionMax2,translationAxis2,velocity2);

	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");

	shared_ptr<FEMSetTextLoaderFunctor> femSetTextLoaderFunctor	= shared_ptr<FEMSetTextLoaderFunctor>(new FEMSetTextLoaderFunctor);
	femSetTextLoaderFunctor->fileName = femTxtFile;

	shared_ptr<PhysicalParametersDispatcher> bodyPhysicalParametersDispatcher(new PhysicalParametersDispatcher);
	bodyPhysicalParametersDispatcher->add("FEMSetParameters","FEMTetrahedronStiffness");
	
	shared_ptr<GeometricalModelDispatcher> geometricalModelDispatcher	= shared_ptr<GeometricalModelDispatcher>(new GeometricalModelDispatcher);
	geometricalModelDispatcher->add("FEMSetParameters","FEMSetGeometry","FEMSet2Tetrahedrons");
	
	shared_ptr<PhysicalParametersDispatcher> positionIntegrator(new PhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	
	shared_ptr<FEMLaw> femLaw(new FEMLaw);
	femLaw->nodeGroupMask = nodeGroupMask;
	femLaw->tetrahedronGroupMask = tetrahedronGroupMask;

	shared_ptr<GravityCondition> gravityCondition(new GravityCondition);
	gravityCondition->gravity = gravity;
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","ParticleParameters","NewtonsForceLaw");
	
	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum"); // FIXME - should be unnecessery, but BUG in ActionParameterVectorVector
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new ActionParameterReset));
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

void FEMBeam::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic			= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<FEMSetParameters> physics(new FEMSetParameters);
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->nodeGroupMask 			= nodeGroupMask;
	physics->tetrahedronGroupMask 		= tetrahedronGroupMask;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
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
 
void FEMBeam::imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity)
{
	shared_ptr<TranslationCondition> translationCondition = shared_ptr<TranslationCondition>(new TranslationCondition);
 	translationCondition->velocity  = velocity;
	direction.normalize();
 	translationCondition->translationAxis = direction;
	
	rootBody->actors.push_back(translationCondition);
	translationCondition->subscribedBodies.clear();
	
	for(rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
	{
		if( rootBody->bodies->getCurrent()->getGroupMask() & nodeGroupMask )
		{
			Vector3r pos = rootBody->bodies->getCurrent()->physicalParameters->se3.position;
			if(        pos[0] > min[0] 
				&& pos[1] > min[1] 
				&& pos[2] > min[2] 
				&& pos[0] < max[0] 
				&& pos[1] < max[1] 
				&& pos[2] < max[2] )
			{
				rootBody->bodies->getCurrent()->isDynamic = false;
				rootBody->bodies->getCurrent()->geometricalModel->diffuseColor = Vector3r(1,0,0);
				translationCondition->subscribedBodies.push_back(rootBody->bodies->getCurrent()->getId());
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////
 
