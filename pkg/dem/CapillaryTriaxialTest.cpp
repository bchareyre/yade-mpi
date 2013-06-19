/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr                                            *
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "CapillaryTriaxialTest.hpp"

#include<yade/pkg/dem/ElasticContactLaw.hpp>
#include <yade/pkg/dem/Law2_ScGeom_CapillaryPhys_Capillarity.hpp>
// #include<yade/pkg/dem/Ip2_FrictMat_FrictMat_FrictPhys.hpp>
#include<yade/pkg/dem/Ip2_FrictMat_FrictMat_CapillaryPhys.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/dem/GlobalStiffnessTimeStepper.hpp>

#include<yade/pkg/dem/TriaxialStressController.hpp>
#include<yade/pkg/dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg/dem/TriaxialStateRecorder.hpp>
#include <yade/pkg/dem/CapillaryStressRecorder.hpp>

#include<yade/pkg/common/Aabb.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/InsertionSortCollider.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/Bo1_Sphere_Aabb.hpp>
#include<yade/pkg/common/Bo1_Box_Aabb.hpp>

#include<yade/pkg/common/GravityEngines.hpp>
#include<yade/pkg/dem/NewtonIntegrator.hpp>

#include<yade/core/Body.hpp>
#include<yade/pkg/common/Box.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#include<yade/pkg/dem/Ig2_Box_Sphere_ScGeom.hpp>

#include<yade/pkg/common/ForceResetter.hpp>

#include<yade/pkg/dem/Shop.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>



using namespace boost;
using namespace std;


typedef pair<Vector3r, Real> BasicSphere;
//! generate a list of non-overlapping spheres
string GenerateCloud_water(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity);


CapillaryTriaxialTest::~CapillaryTriaxialTest () { }



bool CapillaryTriaxialTest::generate(std::string& message)
{
//	unsigned int startId=boost::numeric::bounds<unsigned int>::highest(), endId=0; // record forces from group 2
	message="";	
	scene = shared_ptr<Scene>(new Scene);
	createActors(scene);
	positionRootBody(scene);
	shared_ptr<Body> body;
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
			scene->bodies->insert(body);
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
			 
	}
	
	//convert the original sphere vector (with clump info) to a BasicSphere vector.
	vector<BasicSphere> sphere_list;
	typedef tuple<Vector3r,Real,int> tupleVector3rRealInt;
	if(importFilename!=""){
		vector<tuple<Vector3r,Real,int> >sphereListClumpInfo = Shop::loadSpheresFromFile(importFilename,lowerCorner,upperCorner);
		FOREACH(tupleVector3rRealInt t, sphereListClumpInfo){
			sphere_list.push_back(make_pair(get<0>(t),get<1>(t)));
		};
	}
	else message+=GenerateCloud_water(sphere_list, lowerCorner, upperCorner, numberOfGrains, Rdispersion, 0.75);
	
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
			
	for (;it!=it_end; ++it)
	{
		cerr << "sphere (" << it->first << " " << it->second << ")"<<endl;
		createSphere(body,it->first,it->second,false,true);
		scene->bodies->insert(body);
	}	
	
	return true;
//  	return "Generated a sample inside box of dimensions: (" 
//  		+ lexical_cast<string>(lowerCorner[0]) + "," 
//  		+ lexical_cast<string>(lowerCorner[1]) + "," 
//  		+ lexical_cast<string>(lowerCorner[2]) + ") and (" 
//  		+ lexical_cast<string>(upperCorner[0]) + "," 
//  		+ lexical_cast<string>(upperCorner[1]) + "," 
//  		+ lexical_cast<string>(upperCorner[2]) + ").";

}


void CapillaryTriaxialTest::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius, bool big, bool dynamic )
{
	body = shared_ptr<Body>(new Body); body->groupMask=2;
	shared_ptr<FrictMat> physics(new FrictMat);
	shared_ptr<Aabb> aabb(new Aabb);

	shared_ptr<Sphere> iSphere(new Sphere);
	
	Quaternionr q(Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom());
	q.normalize();
//	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->setDynamic(dynamic);
	
	body->state->angVel		= Vector3r(0,0,0);
	body->state->vel		= Vector3r(0,0,0);
	body->state->mass		= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	
	body->state->inertia		= Vector3r( 	2.0/5.0*body->state->mass*radius*radius,
			2.0/5.0*body->state->mass*radius*radius,
   2.0/5.0*body->state->mass*radius*radius);
	body->state->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= sphereKsDivKn;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	if((!dynamic) && (!boxWalls))
	{
		physics->young			= boxYoungModulus;
		physics->poisson		= boxKsDivKn;
		physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;
	}
	
	aabb->color		= Vector3r(0,1,0);
	
	iSphere->radius			= radius;
	iSphere->color		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	iSphere->wire			= false;
	
	body->shape	= iSphere;
	body->bound		= aabb;
	body->material	= physics;
}


void CapillaryTriaxialTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire)
{
	body = shared_ptr<Body>(new Body); body->groupMask=2;
	shared_ptr<FrictMat> physics(new FrictMat);
	shared_ptr<Aabb> aabb(new Aabb);
	shared_ptr<Box> iBox(new Box);
	
	body->setDynamic(false);
	
	body->state->angVel		= Vector3r(0,0,0);
	body->state->vel		= Vector3r(0,0,0);
	body->state->mass			= 0; 
	//physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	body->state->inertia		= Vector3r(
			body->state->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
			, body->state->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
			, body->state->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						  );
//	physics->mass			= 0;
//	physics->inertia		= Vector3r(0,0,0);
	body->state->se3			= Se3r(position,Quaternionr::Identity());

	physics->young			= boxYoungModulus;
	physics->poisson		= boxKsDivKn;
	physics->frictionAngle		= boxFrictionDeg * Mathr::PI/180.0;

	aabb->color		= Vector3r(1,0,0);
	
	iBox->extents			= extents;
	iBox->color		= Vector3r(1,1,1);
	iBox->wire			= wire;

	body->bound		= aabb;
	body->shape	= iBox;
	body->material	= physics;	
}


void CapillaryTriaxialTest::createActors(shared_ptr<Scene>& scene)
{
	
	Real distanceFactor = 1.3;//Create potential interactions as soon as the distance is less than factor*(rad1+rad2) 
	
	shared_ptr<IGeomDispatcher> interactionGeometryDispatcher(new IGeomDispatcher);
	
	shared_ptr<Ig2_Sphere_Sphere_ScGeom> iS2IS4SContactGeometry(new Ig2_Sphere_Sphere_ScGeom);
	iS2IS4SContactGeometry->interactionDetectionFactor = distanceFactor;//Detect potential distant interaction (meniscii)
	
	interactionGeometryDispatcher->add(iS2IS4SContactGeometry);
	interactionGeometryDispatcher->add(new Ig2_Box_Sphere_ScGeom);

	shared_ptr<IPhysDispatcher> interactionPhysicsDispatcher(new IPhysDispatcher);
//	interactionPhysicsDispatcher->add("Ip2_FrictMat_FrictMat_FrictPhys");
// Unhandled exception: St13runtime_error : Class `Ip2_FrictMat_FrictMat_FrictPhys' could not be cast to required 2D Functor	
	/// OLD
	//interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationshipsWater");
	/// NEW
	shared_ptr<IPhysFunctor> ss(new Ip2_FrictMat_FrictMat_CapillaryPhys);
	interactionPhysicsDispatcher->add(ss);
	
		
	shared_ptr<BoundDispatcher> boundDispatcher	= shared_ptr<BoundDispatcher>(new BoundDispatcher);
	
	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
	shared_ptr<Bo1_Sphere_Aabb> bo1sphere(new Bo1_Sphere_Aabb);
	bo1sphere->aabbEnlargeFactor = distanceFactor;//Detect potential distant interaction (meniscii)
	collider->boundDispatcher->add(bo1sphere);
	collider->boundDispatcher->add(new Bo1_Box_Aabb);

	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt = defaultDt;
	
	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->neverErase = true;
	

	// capillary
	shared_ptr<Law2_ScGeom_CapillaryPhys_Capillarity> capillaryCohesiveLaw(new Law2_ScGeom_CapillaryPhys_Capillarity); 
	capillaryCohesiveLaw->capillaryPressure = capillaryPressure;
// 	capillaryCohesiveLaw->fusionDetection = fusionDetection;
// 	capillaryCohesiveLaw->binaryFusion = binaryFusion;

	//shared_ptr<StiffnessCounter> stiffnesscounter(new StiffnessCounter);
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
	triaxialcompressionEngine->internalCompaction = internalCompaction;
	triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	triaxialcompressionEngine->finalMaxMultiplier = finalMaxMultiplier;
	triaxialcompressionEngine->Key = Key;
	triaxialcompressionEngine->frictionAngleDegree = sphereFrictionDeg;
		
	//cerr << "fin de section triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	
	// recording global stress
	triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new TriaxialStateRecorder);
	triaxialStateRecorder-> file 		= WallStressRecordFile + Key;
	triaxialStateRecorder-> iterPeriod	= recordIntervalIter;
	//triaxialStateRecorderer-> thickness 		= thickness;

	// recording capillary stress
	capillaryStressRecorder = shared_ptr<CapillaryStressRecorder>(new CapillaryStressRecorder);
	capillaryStressRecorder -> file		= capillaryStressRecordFile + Key;
	capillaryStressRecorder -> iterPeriod	= recordIntervalIter;

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
	
	
	scene->engines.clear();
	scene->engines.push_back(shared_ptr<Engine>(new ForceResetter));
//	scene->engines.push_back(sdecTimeStepper);	
	scene->engines.push_back(collider);
	scene->engines.push_back(interactionGeometryDispatcher);
	scene->engines.push_back(interactionPhysicsDispatcher);
	scene->engines.push_back(elasticContactLaw);

	// capillary
	
	
	//scene->engines.push_back(stiffnesscounter);
	//scene->engines.push_back(stiffnessMatrixTimeStepper);
	scene->engines.push_back(globalStiffnessTimeStepper);
	if(water)
	{
		scene->engines.push_back(capillaryCohesiveLaw);
		scene->engines.push_back(capillaryStressRecorder);
	}
	scene->engines.push_back(triaxialcompressionEngine);
	scene->engines.push_back(triaxialStateRecorder);
	//scene->engines.push_back(gravityCondition);
	
	scene->engines.push_back(shared_ptr<Engine> (new NewtonIntegrator));
	
	//if(!rotationBlocked)
	//	scene->engines.push_back(orientationIntegrator);
	//scene->engines.push_back(triaxialstressController);
	
}


void CapillaryTriaxialTest::positionRootBody(shared_ptr<Scene>& scene)
{	
}


string GenerateCloud_water(vector<BasicSphere>& sphere_list, Vector3r lowerCorner, Vector3r upperCorner, long number, Real rad_std_dev, Real porosity)
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
		
	Real mean_radius = std::pow(dimensions.x()*dimensions.y()*dimensions.z()*(1-porosity)/(3.1416*1.3333*number),0.333333);
        //cerr << mean_radius;
        Real Rmin=mean_radius, Rmax=mean_radius;

	std::cerr << "generating aggregates ... ";
	
	long t, i;
	for (i=0; i<number; ++i) {
		BasicSphere s;
		s.second = (random1()-0.5)*rad_std_dev*mean_radius+mean_radius;
		for (t=0; t<tries; ++t) {
			s.first.x() = lowerCorner.x()+s.second+(dimensions.x()-2*s.second)*random1();
			s.first.y() = lowerCorner.y()+s.second+(dimensions.y()-2*s.second)*random1();
			s.first.z() = lowerCorner.z()+s.second+(dimensions.z()-2*s.second)*random1();
			bool overlap=false;
			for (long j=0; (j<i && !overlap); j++)
				if ( pow(sphere_list[j].second+s.second, 2) > (sphere_list[j].first-s.first).squaredNorm()) overlap=true;
			if (!overlap){
				sphere_list.push_back(s);
				Rmin = std::min(Rmin,s.second);
				Rmax = std::max(Rmax,s.second);
				break;}
		}
		if (t==tries) return "More than " + lexical_cast<string>(tries) +
					" tries while generating sphere number " +
					lexical_cast<string>(i+1) + "/" + lexical_cast<string>(number) + ".";
	}
	return "Generated a sample with " + lexical_cast<string>(number) + "spheres inside box of dimensions: (" 
			+ lexical_cast<string>(dimensions[0]) + "," 
			+ lexical_cast<string>(dimensions[1]) + "," 
			+ lexical_cast<string>(dimensions[2]) + ")."
			+ "  mean radius=" + lexical_cast<string>(mean_radius) +
			+ "  Rmin =" + lexical_cast<string>(Rmin) +
			+ "  Rmax =" + lexical_cast<string>(Rmax) + ".";
}



YADE_PLUGIN((CapillaryTriaxialTest));

