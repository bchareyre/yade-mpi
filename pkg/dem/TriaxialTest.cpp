/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include<yade/pkg/dem/ElasticContactLaw.hpp>
#include<yade/pkg/dem/Ip2_FrictMat_FrictMat_FrictPhys.hpp>
#include<yade/pkg/dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/dem/TriaxialStressController.hpp>
#include<yade/pkg/dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg/dem/TriaxialStateRecorder.hpp>
#include<yade/pkg/common/Aabb.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/InsertionSortCollider.hpp>
#include<yade/pkg/common/InsertionSortCollider.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/GravityEngines.hpp>
#include<yade/pkg/dem/NewtonIntegrator.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg/common/Box.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Facet.hpp>
#include<yade/pkg/common/Wall.hpp>
#include<yade/pkg/common/ForceResetter.hpp>
#include<yade/pkg/common/InteractionLoop.hpp>
#include<yade/pkg/dem/Shop.hpp>

#include<yade/pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#include<yade/pkg/dem/Ig2_Box_Sphere_ScGeom.hpp>
#include<yade/pkg/dem/Dem3DofGeom_SphereSphere.hpp>
#include<yade/pkg/dem/Dem3DofGeom_FacetSphere.hpp>
#include<yade/pkg/dem/Dem3DofGeom_WallSphere.hpp>
#include<yade/pkg/dem/Ip2_FrictMat_FrictMat_FrictPhys.hpp>
#include<yade/pkg/common/Bo1_Sphere_Aabb.hpp>
#include<yade/pkg/common/Bo1_Box_Aabb.hpp>
#include<yade/pkg/common/Bo1_Facet_Aabb.hpp>
#include<yade/pkg/common/Wall.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>
// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include<yade/pkg/dem/SpherePack.hpp>
//#include<yade/pkg/dem/MicroMacroAnalyser.hpp>

#include "TriaxialTest.hpp"
CREATE_LOGGER(TriaxialTest);
YADE_PLUGIN((TriaxialTest));

using namespace boost;
using namespace std;

TriaxialTest::~TriaxialTest () {}

bool TriaxialTest::generate(string& message)
{
	message="";
	
	if(biaxial2dTest && (8.0*(upperCorner[2]-lowerCorner[2]))>(upperCorner[0]-lowerCorner[0]))
	{
		message="Biaxial test can be generated only if Z size is more than 8 times smaller than X size";
		return false;
	}
	if(facetWalls&&wallWalls){ LOG_WARN("Turning TriaxialTest::facetWalls off, since wallWalls were selected as well."); }
	
	shared_ptr<Body> body;

	/* if _mean_radius is not given (i.e. <=0), then calculate it from box size;
	 * OTOH, if it is specified, scale the box preserving its ratio and lowerCorner so that the radius can be as requested
	 */
	Real porosity=.8;
	SpherePack sphere_pack;
	if(importFilename==""){
		Vector3r dimensions=upperCorner-lowerCorner; Real volume=dimensions.x()*dimensions.y()*dimensions.z();
		if(radiusMean<=0) radiusMean=pow(volume*(1-porosity)/(Mathr::PI*(4/3.)*numberOfGrains),1/3.);
		else {
			bool fixedDims[3];
			fixedDims[0]=fixedBoxDims.find('x')!=string::npos; fixedDims[1]=fixedBoxDims.find('y')!=string::npos; fixedDims[2]=fixedBoxDims.find('z')!=string::npos;
			int nScaled=(3-(int)fixedDims[0]+(int)fixedDims[1]+(int)fixedDims[2]);
			if(nScaled==0) throw std::invalid_argument("At most 2 (not 3) axes can have fixed dimensions in fixedBoxDims if scaling for given radiusMean.");
			Real boxScaleFactor=radiusMean*pow((4/3.)*Mathr::PI*numberOfGrains/(volume*(1-porosity)),1./nScaled);
			LOG_INFO("Mean radius value of "<<radiusMean<<" requested, scaling "<<nScaled<<" dimensions by "<<boxScaleFactor);
			dimensions[0]*=fixedDims[0]?1.:boxScaleFactor; dimensions[1]*=fixedDims[1]?1.:boxScaleFactor; dimensions[2]*=fixedDims[2]?1.:boxScaleFactor;
			upperCorner=lowerCorner+dimensions;
		}
		long num=sphere_pack.makeCloud(lowerCorner,upperCorner,radiusMean,radiusStdDev,numberOfGrains);
		message+="Generated a sample with " + lexical_cast<string>(num) + " spheres inside box of dimensions: ("
			+ lexical_cast<string>(upperCorner[0]-lowerCorner[0]) + "," 
			+ lexical_cast<string>(upperCorner[1]-lowerCorner[1]) + "," 
			+ lexical_cast<string>(upperCorner[2]-lowerCorner[2]) + ").";}
	else {
		if(radiusMean>0) LOG_WARN("radiusMean ignored, since importFilename specified.");
		sphere_pack.fromFile(importFilename);
		sphere_pack.aabb(lowerCorner,upperCorner);}
	// setup scene here, since radiusMean is now at its true value (if it was negative)
	scene = shared_ptr<Scene>(new Scene);
	positionRootBody(scene);
	createActors(scene);

	if(thickness<0) thickness=radiusMean;
	if(facetWalls || wallWalls) thickness=0;

	// bottom box
	 	Vector3r center		= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						lowerCorner[1]-thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	Vector3r halfSize	= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
							thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_bottom_wire);
	 	if(wall_bottom) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_bottom_id = body->getId();
			//triaxialStateRecorder->wall_bottom_id = body->getId();
			}
		//forcerec->id = body->getId();	
	// top box
	 	center			= Vector3r(	(lowerCorner[0]+upperCorner[0])/2,
	 						upperCorner[1]+thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(	wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
		createBox(body,center,halfSize,wall_top_wire);
	 	if(wall_top) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_top_id = body->getId();
			//triaxialStateRecorder->wall_top_id = body->getId();
			}
	// box 1
	 	center			= Vector3r(
	 						lowerCorner[0]-thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						(lowerCorner[2]+upperCorner[2])/2);
		halfSize		= Vector3r(
							thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
		createBox(body,center,halfSize,wall_1_wire);
	 	if(wall_1) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_left_id = body->getId();
			//triaxialStateRecorder->wall_left_id = body->getId();
			}
	// box 2
	 	center			= Vector3r(
	 						upperCorner[0]+thickness/2.0,
	 						(lowerCorner[1]+upperCorner[1])/2,
							(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	 	
		createBox(body,center,halfSize,wall_2_wire);
	 	if(wall_2) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_right_id = body->getId();
			//triaxialStateRecorder->wall_right_id = body->getId();
			}
	// box 3
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						lowerCorner[2]-thickness/2.0);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_3) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_back_id = body->getId();
			//triaxialStateRecorder->wall_back_id = body->getId();
			}	
	// box 4
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						(lowerCorner[1]+upperCorner[1])/2,
	 						upperCorner[2]+thickness/2.0);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						wallOversizeFactor*fabs(lowerCorner[1]-upperCorner[1])/2+thickness,
	 						thickness/2.0);
		createBox(body,center,halfSize,wall_3_wire);
	 	if(wall_4) {
			scene->bodies->insert(body);
			triaxialcompressionEngine->wall_front_id = body->getId();
			//triaxialStateRecorder->wall_front_id = body->getId();
			}
	size_t imax=sphere_pack.pack.size();
	for(size_t i=0; i<imax; i++){
		const SpherePack::Sph& sp(sphere_pack.pack[i]);
		LOG_DEBUG("sphere (" << sp.c << " " << sp.r << ")");
		createSphere(body,sp.c,sp.r,false,true);
		if(biaxial2dTest){ body->state->blockedDOFs=State::DOF_Z; }
		scene->bodies->insert(body);
	}
	if(defaultDt<0){
		defaultDt=Shop::PWaveTimeStep(scene);
		scene->dt=defaultDt;
		globalStiffnessTimeStepper->defaultDt=defaultDt;
		LOG_INFO("Computed default (PWave) timestep "<<defaultDt);
	}
	return true;
}


void TriaxialTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
{
	body = shared_ptr<Body>(new Body); body->groupMask=2;
	shared_ptr<Aabb> aabb(new Aabb);
	shared_ptr<Sphere> iSphere(new Sphere);
	
	body->setDynamic(dynamic);	
	body->state->mass		= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	body->state->inertia		= Vector3r( 	2.0/5.0*body->state->mass*radius*radius,
							2.0/5.0*body->state->mass*radius*radius,
							2.0/5.0*body->state->mass*radius*radius);
	body->state->pos=position;
	shared_ptr<FrictMat> mat(new FrictMat);
	mat->young			= sphereYoungModulus;
	mat->poisson			= sphereKsDivKn;
	mat->frictionAngle		= compactionFrictionDeg * Mathr::PI/180.0;
	aabb->color		= Vector3r(0,1,0);
	iSphere->radius			= radius;
	//iSphere->color		= Vector3r(0.4,0.1,0.1);
	iSphere->color           = Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	body->shape	= iSphere;
	body->bound	= aabb;
	body->material	= mat;
}


void TriaxialTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body); body->groupMask=2;
	body->setDynamic(false);
	shared_ptr<Aabb> aabb(new Aabb);
	aabb->color		= Vector3r(1,0,0);
	body->bound		= aabb;
	body->state->pos=position;
	shared_ptr<FrictMat> mat(new FrictMat);
	mat->young			= sphereYoungModulus;
	mat->poisson		= sphereKsDivKn;
	mat->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;
	body->material=mat;
	if(!facetWalls && !wallWalls){
		shared_ptr<Box> iBox(new Box);
		iBox->extents			= extents;
		iBox->wire			= wire;
		iBox->color		= Vector3r(1,1,1);
		body->shape	= iBox;
	}
	// guess the orientation
	int ax0 = extents[0]==0 ? 0 : (extents[1]==0 ? 1 : 2); int ax1=(ax0+1)%3, ax2=(ax0+2)%3;
	if(facetWalls){
		Vector3r corner=position-extents; // "lower right" corner, with 90 degrees
		Vector3r side1(Vector3r::Zero()); side1[ax1]=4*extents[ax1]; Vector3r side2(Vector3r::Zero()); side2[ax2]=4*extents[ax2];
		Vector3r v[3]; v[0]=corner; v[1]=corner+side1; v[2]=corner+side2;
		Vector3r cog=Shop::inscribedCircleCenter(v[0],v[1],v[2]);
		shared_ptr<Facet> iFacet(new Facet);
		for(int i=0; i<3; i++){ iFacet->vertices[i]=v[i]-cog;}
		iFacet->color=Vector3r(1,1,1);
		body->shape=iFacet;
	}
	if(wallWalls){
		shared_ptr<Wall> wall(new Wall);
		wall->sense=0; // interact from both sides, since unspecified here
		wall->axis=ax0;
		body->shape=wall;
	}
}


void TriaxialTest::createActors(shared_ptr<Scene>& scene)
{
	
	shared_ptr<IGeomDispatcher> interactionGeometryDispatcher(new IGeomDispatcher);
	if(!facetWalls && !wallWalls){
		interactionGeometryDispatcher->add(new Ig2_Sphere_Sphere_ScGeom);
		interactionGeometryDispatcher->add(new Ig2_Box_Sphere_ScGeom);
	} else {
		interactionGeometryDispatcher->add(new Ig2_Sphere_Sphere_Dem3DofGeom);
		interactionGeometryDispatcher->add(new Ig2_Facet_Sphere_Dem3DofGeom);
		interactionGeometryDispatcher->add(new Ig2_Wall_Sphere_Dem3DofGeom);
	}


	shared_ptr<IPhysDispatcher> interactionPhysicsDispatcher(new IPhysDispatcher);
	shared_ptr<IPhysFunctor> ss(new Ip2_FrictMat_FrictMat_FrictPhys);
	interactionPhysicsDispatcher->add(ss);
	
		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	

	globalStiffnessTimeStepper=shared_ptr<GlobalStiffnessTimeStepper>(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt = defaultDt;
	
	// moving walls to regulate the stress applied + compress when the packing is dense an stable
	//cerr << "triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);
	triaxialcompressionEngine-> stiffnessUpdateInterval = wallStiffnessUpdateInterval;// = stiffness update interval
	triaxialcompressionEngine-> radiusControlInterval = radiusControlInterval;// = stiffness update interval
	triaxialcompressionEngine-> sigmaIsoCompaction = sigmaIsoCompaction;
	triaxialcompressionEngine-> sigmaLateralConfinement = sigmaLateralConfinement;
	triaxialcompressionEngine->max_vel = maxWallVelocity;
	triaxialcompressionEngine-> thickness = thickness;
	triaxialcompressionEngine->strainRate = strainRate;
	triaxialcompressionEngine->StabilityCriterion = StabilityCriterion;
	triaxialcompressionEngine->autoCompressionActivation = autoCompressionActivation;
	triaxialcompressionEngine->autoUnload = autoUnload;
	triaxialcompressionEngine->autoStopSimulation = autoStopSimulation;
	triaxialcompressionEngine->internalCompaction = internalCompaction;
	triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	triaxialcompressionEngine->finalMaxMultiplier = finalMaxMultiplier;
	triaxialcompressionEngine->Key = Key;
	triaxialcompressionEngine->noFiles=noFiles;
	triaxialcompressionEngine->frictionAngleDegree = sphereFrictionDeg;
	triaxialcompressionEngine->fixedPoroCompaction = false;	
	triaxialcompressionEngine->fixedPorosity=1;
	// recording global stress
	if(recordIntervalIter>0 && !noFiles){
		triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new TriaxialStateRecorder);
		triaxialStateRecorder-> file 		= WallStressRecordFile + Key;
		triaxialStateRecorder-> iterPeriod 		= recordIntervalIter;
	}
	#if 0	
	// moving walls to regulate the stress applied, but no loading
	//cerr << "triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);
	triaxialstressController-> stiffnessUpdateInterval = 20;// = recordIntervalIter
	triaxialstressController-> sigma_iso = sigma_iso;
	triaxialstressController-> max_vel = 0.0001;
	triaxialstressController-> thickness = thickness;
	triaxialstressController->wall_bottom_activated = false;
	triaxialstressController->wall_top_activated = false;	
		//cerr << "fin de sezction triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	#endif	
	scene->engines.clear();
	scene->engines.push_back(shared_ptr<Engine>(new ForceResetter));
	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
	scene->engines.push_back(collider);
// 	if(fast){ // The old code was doing the same slower, still here in case we want to make comparisons again
		collider->sweepLength=.05*radiusMean;
		collider->nBins=5; collider->binCoeff=2; /* gives a 2^5=32× difference between the lower and higher bin sweep lengths */
		collider->boundDispatcher->add(new Bo1_Sphere_Aabb);
		collider->boundDispatcher->add(new Bo1_Box_Aabb);
		collider->boundDispatcher->add(new Bo1_Facet_Aabb);
		collider->boundDispatcher->add(new Bo1_Wall_Aabb);

		shared_ptr<InteractionLoop> ids(new InteractionLoop);
			ids->geomDispatcher=interactionGeometryDispatcher;
			ids->physDispatcher=interactionPhysicsDispatcher;
			ids->lawDispatcher=shared_ptr<LawDispatcher>(new LawDispatcher);
			if(!facetWalls && !wallWalls){
				shared_ptr<Law2_ScGeom_FrictPhys_CundallStrack> see(new Law2_ScGeom_FrictPhys_CundallStrack);
				ids->lawDispatcher->add(see);
			} else {
				ids->lawDispatcher->add(shared_ptr<Law2_Dem3DofGeom_FrictPhys_CundallStrack>(new Law2_Dem3DofGeom_FrictPhys_CundallStrack));
			}
		scene->engines.push_back(ids);
	scene->engines.push_back(globalStiffnessTimeStepper);
	scene->engines.push_back(triaxialcompressionEngine);
	if(recordIntervalIter>0 && !noFiles) scene->engines.push_back(triaxialStateRecorder);
	
	shared_ptr<NewtonIntegrator> newton(new NewtonIntegrator);
	newton->damping=dampingMomentum;
	scene->engines.push_back(newton);
}

void TriaxialTest::positionRootBody(shared_ptr<Scene>& scene)
{
}
