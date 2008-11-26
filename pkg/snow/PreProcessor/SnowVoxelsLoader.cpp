/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg-dem/CohesiveFrictionalContactLaw.hpp>
#include<yade/pkg-dem/CohesiveFrictionalRelationships.hpp>
#include<yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/pkg-dem/GlobalStiffnessCounter.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>

#include<yade/pkg-dem/AveragePositionRecorder.hpp>
#include<yade/pkg-dem/ForceRecorder.hpp>
#include<yade/pkg-dem/VelocityRecorder.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg-dem/TriaxialStateRecorder.hpp>

#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/SAPCollider.hpp>
#include<yade/pkg-common/DistantPersistentSAPCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-dem/HydraulicForceEngine.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>

#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>

#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/InteractionHashMap.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>

#include<yade/extra/Shop.hpp>

#include<boost/filesystem/convenience.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/numeric/conversion/bounds.hpp>
#include<boost/limits.hpp>


#include"SnowVoxelsLoader.hpp"
#include<set>

SnowVoxelsLoader::SnowVoxelsLoader() : FileGenerator()
{
	voxel_binary_data_file = "/home/janek/32-Snow-white/20-Programy/31-SNOW-read-data/RESULT.bz2";
//	voxel_txt_dir = "";
//	voxel_caxis_file = "";
//	voxel_colors_file = "";
	grain_binary_data_file = "grain_binary.bz2";
	
	density			= 2600;
	sphereYoungModulus	= 15000000.0;
	spherePoissonRatio	= 0.5;
	sphereFrictionDeg	= 18.0;
	spheresColor		= Vector3r(0.8,0.3,0.3);

	m_grains.clear();
}


SnowVoxelsLoader::~SnowVoxelsLoader ()
{
}

void SnowVoxelsLoader::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(voxel_binary_data_file);
	REGISTER_ATTRIBUTE(voxel_txt_dir);
	REGISTER_ATTRIBUTE(voxel_caxis_file);
	REGISTER_ATTRIBUTE(voxel_colors_file);
	REGISTER_ATTRIBUTE(grain_binary_data_file);
}

bool SnowVoxelsLoader::load_voxels()
{
	if(grain_binary_data_file !="" && boost::filesystem::exists(grain_binary_data_file))
	{
		std::cerr << "no need to load voxels - grain binary file exists\n";
		std::cerr << "loading " << grain_binary_data_file << " ...";
			boost::iostreams::filtering_istream ifs;
			ifs.push(boost::iostreams::bzip2_decompressor());
			ifs.push(boost::iostreams::file_source(grain_binary_data_file));
		//std::ifstream ifs(m_config.load_file());
		boost::archive::binary_iarchive ia(ifs);
		ia >> m_grains;
		return true;
	}

	if(voxel_binary_data_file != "" && boost::filesystem::exists(voxel_binary_data_file))
	{
		std::cerr << "loading " << voxel_binary_data_file << " ...";
			boost::iostreams::filtering_istream ifs;
			ifs.push(boost::iostreams::bzip2_decompressor());
			ifs.push(boost::iostreams::file_source(voxel_binary_data_file));
		//std::ifstream ifs(m_config.load_file());
		boost::archive::binary_iarchive ia(ifs);
		ia >> m_voxel;
		std::cerr << " finished\n";
		return true;
	}
	else
	{
		message="cannot load txt yet, or nothing to load";
		return false;
	}
	return false;
}

bool SnowVoxelsLoader::generate()
{
	if(!load_voxels())
		return false;

	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);
	
	if(m_grains.size() == 0)
	{
		const T_DATA& dat(m_voxel.m_data.get_data_voxel_const_ref().get_a_voxel_const_ref());
		std::set<unsigned char> done;done.clear();done.insert(0);
		BOOST_FOREACH(const std::vector<std::vector<unsigned char> >& a,dat)
			BOOST_FOREACH(const std::vector<unsigned char>& b,a)
				BOOST_FOREACH(unsigned char c,b)
				{
					if(done.find(c)==done.end())
					{
						done.insert(c);
						boost::shared_ptr<BshSnowGrain> grain(new BshSnowGrain(dat,m_voxel.m_data.get_axes_const_ref()[c],c,m_voxel.m_data.get_colors_const_ref()[c]));
						m_grains.push_back(grain);
					}
				};

		std::cerr << "saving "<< grain_binary_data_file << " ...";
		boost::iostreams::filtering_ostream ofs;
		ofs.push(boost::iostreams::bzip2_compressor());
		ofs.push(boost::iostreams::file_sink(grain_binary_data_file));
		boost::archive::binary_oarchive oa(ofs);
		oa << m_grains;
		std::cerr << " finished\n";
	}
	
	shared_ptr<Body> body;
	BOOST_FOREACH(boost::shared_ptr<BshSnowGrain> grain,m_grains)
	{
		create_grain(body,grain->center,(grain->start-grain->end).Length()*0.5,true,grain);
		rootBody->bodies->insert(body);
	}
	
	return true;
}

void SnowVoxelsLoader::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	physicalActionInitializer->physicalActionNames.push_back("GlobalStiffness");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	shared_ptr<InteractionGeometryEngineUnit> s1(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->add(s1);
	shared_ptr<InteractionGeometryEngineUnit> s2(new InteractingBox2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->add(s2);

	shared_ptr<CohesiveFrictionalRelationships> cohesiveFrictionalRelationships = shared_ptr<CohesiveFrictionalRelationships> (new CohesiveFrictionalRelationships);
	//cohesiveFrictionalRelationships->shearCohesion = shearCohesion;
	//cohesiveFrictionalRelationships->normalCohesion = normalCohesion;
	//cohesiveFrictionalRelationships->setCohesionOnNewContacts = setCohesionOnNewContacts;
	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add(cohesiveFrictionalRelationships);
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry2AABB");

	

		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	//gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	//actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	//actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add(actionForceDamping);
	actionDampingDispatcher->add(actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("NewtonsForceLaw");
	applyActionDispatcher->add("NewtonsMomentumLaw");
		
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("LeapFrogOrientationIntegrator");

	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask = 2;
	//globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	//globalStiffnessTimeStepper->defaultDt = defaultDt;
	globalStiffnessTimeStepper->timestepSafetyCoefficient = 0.2;
	
	shared_ptr<CohesiveFrictionalContactLaw> cohesiveFrictionalContactLaw(new CohesiveFrictionalContactLaw);
	cohesiveFrictionalContactLaw->sdecGroupMask = 2;
	cohesiveFrictionalContactLaw->shear_creep = false;
	cohesiveFrictionalContactLaw->twist_creep = false;
	//cohesiveFrictionalContactLaw->creep_viscosity = creep_viscosity;
	
	//shared_ptr<StiffnessCounter> stiffnesscounter(new StiffnessCounter);
	//stiffnesscounter->sdecGroupMask = 2;
	//stiffnesscounter->interval = timeStepUpdateInterval;
	
	shared_ptr<GlobalStiffnessCounter> globalStiffnessCounter(new GlobalStiffnessCounter);
	// globalStiffnessCounter->sdecGroupMask = 2;
	//globalStiffnessCounter->interval = timeStepUpdateInterval;
	
	// moving walls to regulate the stress applied + compress when the packing is dense an stable
	//cerr << "triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	//triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);
	//triaxialcompressionEngine-> stiffnessUpdateInterval = wallStiffnessUpdateInterval;// = stiffness update interval
	//triaxialcompressionEngine-> radiusControlInterval = radiusControlInterval;// = stiffness update interval
	//triaxialcompressionEngine-> sigma_iso = sigma_iso;
	//triaxialcompressionEngine-> sigmaLateralConfinement = sigma_iso;
	//triaxialcompressionEngine-> sigmaIsoCompaction = sigma_iso;
	//triaxialcompressionEngine-> max_vel = 1;
	//triaxialcompressionEngine-> thickness = thickness;
	//triaxialcompressionEngine->strainRate = strainRate;
	//triaxialcompressionEngine->StabilityCriterion = StabilityCriterion;
	//triaxialcompressionEngine->autoCompressionActivation = autoCompressionActivation;
	//triaxialcompressionEngine->internalCompaction = internalCompaction;
	//triaxialcompressionEngine->maxMultiplier = maxMultiplier;
	
	shared_ptr<HydraulicForceEngine> hydraulicForceEngine = shared_ptr<HydraulicForceEngine> (new HydraulicForceEngine);
	hydraulicForceEngine->dummyParameter = true;
		
	//cerr << "fin de section triaxialcompressionEngine = shared_ptr<TriaxialCompressionEngine> (new TriaxialCompressionEngine);" << std::endl;
	
// recording global stress
	//triaxialStateRecorder = shared_ptr<TriaxialStateRecorder>(new TriaxialStateRecorder);
	//triaxialStateRecorder-> outputFile 	= WallStressRecordFile;
	//triaxialStateRecorder-> interval 		= recordIntervalIter;
	//triaxialStateRecorder-> thickness 		= thickness;
	
	
	// moving walls to regulate the stress applied
	//cerr << "triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	//triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);
	//triaxialstressController-> stiffnessUpdateInterval = 20;// = recordIntervalIter
	//triaxialstressController-> sigma_iso = sigma_iso;
	//triaxialstressController-> max_vel = 0.0001;
	//triaxialstressController-> thickness = thickness;
	//triaxialstressController->wall_bottom_activated = false;
	//triaxialstressController->wall_top_activated = false;	
		//cerr << "fin de sezction triaxialstressController = shared_ptr<TriaxialStressController> (new TriaxialStressController);" << std::endl;
	
	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
//	rootBody->engines.push_back(sdecTimeStepper);	
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new DistantPersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(cohesiveFrictionalContactLaw);
	///rootBody->engines.push_back(triaxialcompressionEngine);
	//rootBody->engines.push_back(stiffnesscounter);
	//rootBody->engines.push_back(stiffnessMatrixTimeStepper);
	rootBody->engines.push_back(globalStiffnessCounter);
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	///rootBody->engines.push_back(triaxialStateRecorder);
	rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	//if(!rotationBlocked)
		rootBody->engines.push_back(orientationIntegrator);
	//rootBody->engines.push_back(resultantforceEngine);
	//rootBody->engines.push_back(triaxialstressController);
	
		
	//rootBody->engines.push_back(averagePositionRecorder);
	//rootBody->engines.push_back(velocityRecorder);
	//rootBody->engines.push_back(forcerec);
	
	//if (saveAnimationSnapshots) {
	//shared_ptr<PositionOrientationRecorder> positionOrientationRecorder(new PositionOrientationRecorder);
	//positionOrientationRecorder->outputFile = AnimationSnapshotsBaseName;
	//rootBody->engines.push_back(positionOrientationRecorder);}
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	
}

void SnowVoxelsLoader::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->mass			= 0;
	physics->velocity		= Vector3r::ZERO;
	physics->acceleration		= Vector3r::ZERO;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);
	
	rootBody->interactingGeometry	= YADE_PTR_CAST<InteractingGeometry>(set);	
	rootBody->boundingVolume	= YADE_PTR_CAST<BoundingVolume>(aabb);
	rootBody->physicalParameters 	= physics;
	
}

void SnowVoxelsLoader::create_grain(shared_ptr<Body>& body, Vector3r position, Real radius, bool dynamic , boost::shared_ptr<BshSnowGrain> grain)
{
	body = shared_ptr<Body>(new Body(body_id_t(0),2));
	shared_ptr<CohesiveFrictionalBodyParameters> physics(new CohesiveFrictionalBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<BshSnowGrain> gSnowGrain(grain);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;//(Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom());
	q.FromAxisAngle( Vector3r(0,0,1),0);
	q.Normalize();
	
	body->isDynamic			= dynamic;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	
	physics->inertia		= Vector3r( 	2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius,
							2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);


	//gSnowGrain->radius		= radius;
//	gSnowGrain->diffuseColor	= ((int)(position[0]*400.0))%2?Vector3r(0.7,0.7,0.7):Vector3r(0.45,0.45,0.45);
//	gSnowGrain->diffuseColor	= spheresColor;
	gSnowGrain->diffuseColor	= grain->color;
	gSnowGrain->wire		= false;
	gSnowGrain->visible		= true;
	gSnowGrain->shadowCaster	= true;
	
	iSphere->radius			= radius;
//	iSphere->diffuseColor		= Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom());
	iSphere->diffuseColor		= grain->color;

	body->interactingGeometry	= iSphere;
	body->geometricalModel		= gSnowGrain;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

