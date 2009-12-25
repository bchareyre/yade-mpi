/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "TriaxialTest.hpp"

#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/SimpleElasticRelationships.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>

#include<yade/pkg-common/ElasticMat.hpp>

//#include<yade/pkg-dem/AveragePositionRecorder.hpp>
//#include<yade/pkg-dem/ForceRecorder.hpp>
//#include<yade/pkg-dem/VelocityRecorder.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg-dem/TriaxialStateRecorder.hpp>

#include<yade/pkg-common/Aabb.hpp>
#ifdef YADE_GEOMETRICALMODEL
	#include<yade/pkg-common/BoxModel.hpp>
	#include<yade/pkg-common/SphereModel.hpp>
	#include<yade/pkg-common/FacetModel.hpp>
#endif
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundDispatcher.hpp>

#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-dem/NewtonIntegrator.hpp>

#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Facet.hpp>
#include<yade/pkg-common/Wall.hpp>

#include<yade/pkg-common/ForceResetter.hpp>

#include<yade/pkg-common/InteractionDispatchers.hpp>

#include<yade/pkg-dem/Shop.hpp>


#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>

#include<yade/pkg-dem/SpherePack.hpp>

//#include<yade/pkg-dem/MicroMacroAnalyser.hpp>


CREATE_LOGGER(TriaxialTest);

using namespace boost;
using namespace std;




TriaxialTest::TriaxialTest () : FileGenerator()
{
	lowerCorner 		= Vector3r(0,0,0);
	upperCorner 		= Vector3r(1,1,1);
	thickness 		= 0.001;
	importFilename          = ""; // oh, PLEASE, keep this empty -- i.e., by default, generate spheres in the box, not load them.
	Key			="";
	outputFileName 		= "./TriaxialTest"+Key+".xml";
	//nlayers = 1;
	wall_top 		= true;
	wall_bottom 		= true;
	wall_1			= true;
	wall_2			= true;
	wall_3			= true;
	wall_4			= true;
	wall_top_wire 		= true;
	wall_bottom_wire	= true;
	wall_1_wire		= true;
	wall_2_wire		= true;
	wall_3_wire		= true;
	wall_4_wire		= true;
	spheresColor		= Vector3r(0.8,0.3,0.3);
	spheresRandomColor	= false;
	recordIntervalIter	= 20;
	saveAnimationSnapshots = false;
	AnimationSnapshotsBaseName = "./snapshots_"+Key+"/snap";
	WallStressRecordFile = "./WallStresses"+Key;

	rotationBlocked = false;
	//	boxWalls 		= false;
	boxWalls 		= true;
	internalCompaction	=false;
	
	dampingForce = 0.2;
	dampingMomentum = 0.2;
	defaultDt = -1;
	
	timeStepUpdateInterval = 50;
	timeStepOutputInterval = 50;
	wallStiffnessUpdateInterval = 10;
	radiusControlInterval = 10;
	numberOfGrains = 400;
	strainRate = 0.1;
	maxWallVelocity=10;
	StabilityCriterion = 0.01;
	autoCompressionActivation = true;
	autoUnload = true;
	autoStopSimulation = false;
	maxMultiplier = 1.01;
	finalMaxMultiplier = 1.001;
	
	sphereYoungModulus  = 15000000.0;
	spherePoissonRatio  = 0.5;	
	sphereFrictionDeg = 18.0;
	compactionFrictionDeg   = sphereFrictionDeg;
	density			= 2600;
	
	boxYoungModulus   = 15000000.0;
	boxPoissonRatio  = 0.2;
	boxFrictionDeg   = 0.f;
	gravity 	= Vector3r(0,-9.81,0);
	
	sigmaIsoCompaction = 50000;
	sigmaLateralConfinement=sigmaIsoCompaction;

	wallOversizeFactor=1.3;

	biaxial2dTest=false;

	radiusStdDev=0.3;
	radiusMean=-1; // no radius specified
	
	isotropicCompaction=false;
	fixedPorosity = 1;
	
	fast=false;
	noFiles=false;
	facetWalls=false;
	wallWalls=false;

	
	
//	wall_top_id =0;
// 	wall_bottom_id =0;
// 	wall_left_id =0;
// 	all_right_id =0;
// 	wall_front_id =0;
// 	wall_back_id =0;
}


TriaxialTest::~TriaxialTest ()
{

}



bool TriaxialTest::generate()
{
//	unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	message="";
	
	if(biaxial2dTest && (8.0*(upperCorner[2]-lowerCorner[2]))>(upperCorner[0]-lowerCorner[0]))
	{
		message="Biaxial test can be generated only if Z size is more than 8 times smaller than X size";
		return false;
	}
	if((facetWalls||wallWalls) && !fast){
		LOG_WARN("Turning TriaxialTest::fast on, since facetWalls or wallWalls were selected.");
		fast=true;
	}
	if(facetWalls&&wallWalls){ LOG_WARN("Turning TriaxialTest::facetWalls off, since wallWalls were selected as well."); }
	
	shared_ptr<Body> body;

	/* if _mean_radius is not given (i.e. <=0), then calculate it from box size;
	 * OTOH, if it is specified, scale the box preserving its ratio and lowerCorner so that the radius can be as requested
	 */
	Real porosity=.75;
	SpherePack sphere_pack;
	if(importFilename==""){
		Vector3r dimensions=upperCorner-lowerCorner; Real volume=dimensions.X()*dimensions.Y()*dimensions.Z();
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
			+ lexical_cast<string>(upperCorner[2]-lowerCorner[2]) + ").";
		
	}
	else {
		if(radiusMean>0) LOG_WARN("radiusMean ignored, since importFilename specified.");
		sphere_pack.fromFile(importFilename);
		sphere_pack.aabb(lowerCorner,upperCorner);
	}

	// setup rootBody here, since radiusMean is now at its true value (if it was negative)
	rootBody = shared_ptr<Scene>(new Scene);
	positionRootBody(rootBody);
	createActors(rootBody);


	if(thickness<0) thickness=radiusMean;
	if(facetWalls || wallWalls) thickness=0;
	
	if(boxWalls)
	{
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
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_bottom_id = body->getId();
			//triaxialStateRecorder->wall_bottom_id = body->getId();
			}
		//forcerec->id = body->getId();
	
	// top box
	 	center			= Vector3r(
	 						(lowerCorner[0]+upperCorner[0])/2,
	 						upperCorner[1]+thickness/2.0,
	 						(lowerCorner[2]+upperCorner[2])/2);
	 	halfSize		= Vector3r(
	 						wallOversizeFactor*fabs(lowerCorner[0]-upperCorner[0])/2+thickness,
	 						thickness/2.0,
	 						wallOversizeFactor*fabs(lowerCorner[2]-upperCorner[2])/2+thickness);
	
		createBox(body,center,halfSize,wall_top_wire);
	 	if(wall_top) {
			rootBody->bodies->insert(body);
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
			rootBody->bodies->insert(body);
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
			rootBody->bodies->insert(body);
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
			rootBody->bodies->insert(body);
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
			rootBody->bodies->insert(body);
			triaxialcompressionEngine->wall_front_id = body->getId();
			//triaxialStateRecorder->wall_front_id = body->getId();
			}
			 
	}

	size_t imax=sphere_pack.pack.size();
	for(size_t i=0; i<imax; i++){
		const SpherePack::Sph& sp(sphere_pack.pack[i]);
		LOG_DEBUG("sphere (" << sp.c << " " << sp.r << ")");
		createSphere(body,sp.c,sp.r,false,true);
		if(biaxial2dTest){ body->state->blockedDOFs=State::DOF_Z; }
		rootBody->bodies->insert(body);
	}	

	if(defaultDt<0){
		defaultDt=Shop::PWaveTimeStep(rootBody);
		rootBody->dt=defaultDt;
		globalStiffnessTimeStepper->defaultDt=defaultDt;
		LOG_INFO("Computed default (PWave) timestep "<<defaultDt);
	}

	return true;
}


void TriaxialTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
{
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
	//shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<Aabb> aabb(new Aabb);
	shared_ptr<Sphere> iSphere(new Sphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= dynamic;
	
	body->state->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	
	body->state->inertia		= Vector3r( 	2.0/5.0*body->state->mass*radius*radius,
							2.0/5.0*body->state->mass*radius*radius,
							2.0/5.0*body->state->mass*radius*radius);
	body->state->pos=position;

	shared_ptr<GranularMat> mat(new GranularMat);
	mat->young			= sphereYoungModulus;
	mat->poisson		= spherePoissonRatio;
	mat->frictionAngle		= compactionFrictionDeg * Mathr::PI/180.0;

// 	if((!big) && (!dynamic) && (!boxWalls))
// 	{
// 		physics->young			= boxYoungModulus;
// 		physics->poisson		= boxPoissonRatio;
// 		physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;
// 	}
	
	aabb->diffuseColor		= Vector3r(0,1,0);

	#ifdef YADE_GEOMETRICALMODEL
		shared_ptr<SphereModel> gSphere(new SphereModel);
		gSphere->radius			= radius;
		gSphere->diffuseColor		= spheresColor;
		gSphere->wire			= false;
		gSphere->shadowCaster		= true;
		body->geometricalModel		= gSphere;
	#endif
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());

	body->shape	= iSphere;
	body->bound		= aabb;
	body->material	= mat;
}


void TriaxialTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
	body->isDynamic			= false;


	shared_ptr<Aabb> aabb(new Aabb);
	aabb->diffuseColor		= Vector3r(1,0,0);
	body->bound		= aabb;
	
	/* FIXME?? mass is not assigned (zero), is that OK?
	body->state->inertia		= Vector3r(
							  body->state->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, body->state->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, body->state->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	*/
	body->state->pos=position;

	shared_ptr<GranularMat> mat(new GranularMat);
	mat->young			= sphereYoungModulus;
	mat->poisson		= spherePoissonRatio;
	mat->frictionAngle		= compactionFrictionDeg * Mathr::PI/180.0;
	body->material=mat;


	if(!facetWalls && !wallWalls){
		#ifdef YADE_GEOMETRICALMODEL
			shared_ptr<BoxModel> gBox(new BoxModel);
			gBox->extents			= extents;
			gBox->diffuseColor		= Vector3r(1,1,1);
			gBox->wire			= wire;
			gBox->shadowCaster		= false;
			body->geometricalModel		= gBox;
		#endif

		shared_ptr<Box> iBox(new Box);
		iBox->extents			= extents;
		iBox->wire			= wire;
		iBox->diffuseColor		= Vector3r(1,1,1);
		body->shape	= iBox;
	}
	// guess the orientation
	int ax0 = extents[0]==0 ? 0 : (extents[1]==0 ? 1 : 2); int ax1=(ax0+1)%3, ax2=(ax0+2)%3;
	if(facetWalls){
		Vector3r corner=position-extents; // "lower right" corner, with 90 degrees
		Vector3r side1(Vector3r::ZERO); side1[ax1]=4*extents[ax1]; Vector3r side2(Vector3r::ZERO); side2[ax2]=4*extents[ax2];
		Vector3r v[3]; v[0]=corner; v[1]=corner+side1; v[2]=corner+side2;
		Vector3r cog=Shop::inscribedCircleCenter(v[0],v[1],v[2]);
		shared_ptr<Facet> iFacet(new Facet);
		for(int i=0; i<3; i++){ iFacet->vertices.push_back(v[i]-cog);}
		iFacet->diffuseColor=Vector3r(1,1,1);
		body->shape=iFacet;
		#ifdef YADE_GEOMETRICALMODEL
			shared_ptr<FacetModel> facet(new FacetModel);
			for(int i=0; i<3; i++){ facet->vertices.push_back(v[i]-cog);}
			facet->wire=true;
			body->geometricalModel=facet;
		#endif
	}
	if(wallWalls){
		shared_ptr<Wall> wall(new Wall);
		wall->sense=0; // interact from both sides, since unspecified here
		wall->axis=ax0;
		// Wall has no geometricalModel, skip assignment to body->geometricalModel
		body->shape=wall;
	}
}


void TriaxialTest::createActors(shared_ptr<Scene>& rootBody)
{
	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	if(!facetWalls && !wallWalls){
		interactionGeometryDispatcher->add("Ig2_Sphere_Sphere_ScGeom");
		interactionGeometryDispatcher->add("Ig2_Box_Sphere_ScGeom");
	} else {
		interactionGeometryDispatcher->add("Ig2_Sphere_Sphere_Dem3DofGeom");
		interactionGeometryDispatcher->add("Ig2_Facet_Sphere_Dem3DofGeom");
		interactionGeometryDispatcher->add("Ig2_Wall_Sphere_Dem3DofGeom");
	}


	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	shared_ptr<InteractionPhysicsFunctor> ss(new SimpleElasticRelationships);
	interactionPhysicsDispatcher->add(ss);
	
		
	shared_ptr<BoundDispatcher> boundDispatcher	= shared_ptr<BoundDispatcher>(new BoundDispatcher);
	boundDispatcher->add("Bo1_Sphere_Aabb");
	boundDispatcher->add("Bo1_Box_Aabb");
	boundDispatcher->add("Bo1_Facet_Aabb");
	boundDispatcher->add("Bo1_Wall_Aabb");
		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	

	globalStiffnessTimeStepper=shared_ptr<GlobalStiffnessTimeStepper>(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask = 2;
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt = defaultDt;
	
	//shared_ptr<StiffnessCounter> stiffnesscounter(new StiffnessCounter);
	//stiffnesscounter->sdecGroupMask = 2;
	//stiffnesscounter->interval = timeStepUpdateInterval;
	
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
	triaxialcompressionEngine->fixedPorosity = fixedPorosity;
	triaxialcompressionEngine->isotropicCompaction = isotropicCompaction;
	
	
	// recording global stress
	if(recordIntervalIter>0 && !noFiles){
		triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new TriaxialStateRecorder);
		triaxialStateRecorder-> file 		= WallStressRecordFile + Key;
		triaxialStateRecorder-> iterPeriod 		= recordIntervalIter;
		//triaxialStateRecorderer-> thickness 		= thickness;
	}
	

	#if 0	
	// moving walls to regulate the stress applied
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
	
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new ForceResetter));
	rootBody->engines.push_back(boundDispatcher);
	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
	rootBody->engines.push_back(collider);
	if(fast){
		collider->sweepLength=.05*radiusMean;
		collider->nBins=5; collider->binCoeff=2; /* gives a 2^5=32× difference between the lower and higher bin sweep lengths */
		shared_ptr<InteractionDispatchers> ids(new InteractionDispatchers);
			ids->geomDispatcher=interactionGeometryDispatcher;
			ids->physDispatcher=interactionPhysicsDispatcher;
			ids->lawDispatcher=shared_ptr<LawDispatcher>(new LawDispatcher);
			if(!facetWalls && !wallWalls){
				shared_ptr<ef2_Spheres_Elastic_ElasticLaw> see(new ef2_Spheres_Elastic_ElasticLaw); see->sdecGroupMask=2;
				ids->lawDispatcher->add(see);
			} else {
				ids->lawDispatcher->add(shared_ptr<Law2_Dem3Dof_Elastic_Elastic>(new Law2_Dem3Dof_Elastic_Elastic));
			}
		rootBody->engines.push_back(ids);
	} else {
		assert(!facetWalls);
		rootBody->engines.push_back(interactionGeometryDispatcher);
		rootBody->engines.push_back(interactionPhysicsDispatcher);
		shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
		elasticContactLaw->sdecGroupMask = 2;
		rootBody->engines.push_back(elasticContactLaw);
	}
	
	//rootBody->engines.push_back(stiffnesscounter);
	//rootBody->engines.push_back(stiffnessMatrixTimeStepper);
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	rootBody->engines.push_back(triaxialcompressionEngine);
	if(recordIntervalIter>0 && !noFiles) rootBody->engines.push_back(triaxialStateRecorder);
	//rootBody->engines.push_back(gravityCondition);
	
	shared_ptr<NewtonIntegrator> newton(new NewtonIntegrator);
	newton->damping=dampingMomentum;
	rootBody->engines.push_back(newton);
	
	//if (0) rootBody->engines.push_back(shared_ptr<Engine>(new MicroMacroAnalyser));
	
	//if(!rotationBlocked)
	//	rootBody->engines.push_back(orientationIntegrator);
	//rootBody->engines.push_back(triaxialstressController);
	
		
	if (saveAnimationSnapshots) {
		shared_ptr<PositionOrientationRecorder> positionOrientationRecorder(new PositionOrientationRecorder);
		positionOrientationRecorder->outputFile = AnimationSnapshotsBaseName;
		rootBody->engines.push_back(positionOrientationRecorder);
	}
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundDispatcher);
	
}


void TriaxialTest::positionRootBody(shared_ptr<Scene>& rootBody)
{
}
// 0xdeadc0de, superseded by SpherePack::makeCloud
#if 0
string TriaxialTest::GenerateCloud(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real mean_radius, Real porosity)
{
	typedef boost::minstd_rand StdGenerator;
	static StdGenerator generator;
	static boost::variate_generator<StdGenerator&, boost::uniform_real<> >
			random1(generator, boost::uniform_real<>(0,1));
        //         static boost::variate_generator<StdGenerator&, boost::normal_distribution<> >
        //         randomN(generator, boost::normal_distribution<>(aggregateMeanRadius,aggregateSigmaRadius));

	sphere_list.clear();
	long tries = 1000; //nb of tries for positionning the next sphere
	Vector3r dimensions = upperCorner - lowerCorner;

	LOG_INFO("Generating "<<number<<" aggregates ...");
	
	long t, i;
	for (i=0; i<number; ++i) {
		BasicSphere s;
		for (t=0; t<tries; ++t) {
			s.second = (random1()-0.5)*rad_std_dev*mean_radius+mean_radius;
			s.first.X() = lowerCorner.X()+s.second+(dimensions.X()-2*s.second)*random1();
			s.first.Y() = lowerCorner.Y()+s.second+(dimensions.Y()-2*s.second)*random1();
			s.first.Z() = lowerCorner.Z()+s.second+(dimensions.Z()-2*s.second)*random1();
			bool overlap=false;
			for (long j=0; (j<i && !overlap); j++)
				if ( pow(sphere_list[j].second+s.second, 2) > (sphere_list[j].first-s.first).SquaredLength()) overlap=true;
			if (!overlap)
			{
				sphere_list.push_back(s);
				break;
			}			
		}
		if (t==tries) return "More than " + lexical_cast<string>(tries) +
					" tries while generating sphere number " +
					lexical_cast<string>(i+1) + "/" + lexical_cast<string>(number) + ".";
	}
	return "Generated a sample with " + lexical_cast<string>(number) + " spheres inside box of dimensions: (" 
			+ lexical_cast<string>(dimensions[0]) + "," 
			+ lexical_cast<string>(dimensions[1]) + "," 
			+ lexical_cast<string>(dimensions[2]) + ").";
}
#endif


YADE_PLUGIN((TriaxialTest));
