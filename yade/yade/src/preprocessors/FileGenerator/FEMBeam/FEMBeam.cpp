#include "FEMBeam.hpp"


#include "AABB.hpp"
#include "Sphere.hpp"
#include "Tetrahedron.hpp"
#include "FEMSetParameters.hpp"
#include "FEMTetrahedronData.hpp"
#include "FEMSetTextLoaderFunctor.hpp"
#include "FEMNodeData.hpp"
#include "InteractionDescriptionSet.hpp"

#include "ActionParameterReset.hpp"
#include "ActionParameterInitializer.hpp"
#include "CundallNonViscousForceDampingFunctor.hpp"
#include "CundallNonViscousMomentumDampingFunctor.hpp"

#include "GravityCondition.hpp"

// body
#include "ComplexBody.hpp"
#include "SimpleBody.hpp"

// dispatchers
#include "BodyPhysicalParametersDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "BoundingVolumeDispatcher.hpp"

#include <boost/filesystem/convenience.hpp>

using namespace boost;
using namespace std;

FEMBeam::FEMBeam () : FileGenerator()
{
	gravity 		= Vector3r(0,-9.81,0);
	femTxtFile 		= "../data/fem.beam";
	nodeGroupMask 		= 1;
	tetrahedronGroupMask 	= 2;
}

FEMBeam::~FEMBeam ()
{ 
}

void FEMBeam::postProcessAttributes(bool)
{
}

void FEMBeam::registerAttributes()
{
	REGISTER_ATTRIBUTE(femTxtFile);
	REGISTER_ATTRIBUTE(gravity);
}

string FEMBeam::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::createActors(shared_ptr<ComplexBody>& rootBody)
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

	shared_ptr<FEMSetTextLoaderFunctor> femSetTextLoaderFunctor	= shared_ptr<FEMSetTextLoaderFunctor>(new FEMSetTextLoaderFunctor);
	femSetTextLoaderFunctor->fileName = femTxtFile;
	shared_ptr<BodyPhysicalParametersDispatcher> bodyPhysicalParametersDispatcher(new BodyPhysicalParametersDispatcher);
	bodyPhysicalParametersDispatcher->add("FEMSetParameters","FEMSetTextLoaderFunctor",femSetTextLoaderFunctor);
	
	rootBody->actors.clear();
	rootBody->actors.push_back(boundingVolumeDispatcher);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(bodyPhysicalParametersDispatcher);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::positionRootBody(shared_ptr<ComplexBody>& rootBody) 
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
 
