/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "SimpleShear.hpp"

#include<pkg/dem/GlobalStiffnessTimeStepper.hpp>
#include<pkg/dem/ElasticContactLaw.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/common/Aabb.hpp>
#include<core/Scene.hpp>
#include<pkg/common/InsertionSortCollider.hpp>
#include<core/Interaction.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/InteractionLoop.hpp>

#include<pkg/common/ForceResetter.hpp>

#include<pkg/dem/NewtonIntegrator.hpp>
#include<pkg/common/GravityEngines.hpp>
#include<pkg/dem/KinemC__Engine.hpp>

#include<pkg/dem/Ig2_Sphere_Sphere_ScGeom.hpp>
#include<pkg/dem/Ig2_Box_Sphere_ScGeom.hpp>
#include<pkg/common/Bo1_Aabb.hpp>
#include<pkg/common/Bo1_Aabb.hpp>

#include<core/Body.hpp>
#include<pkg/common/Box.hpp>
#include<pkg/common/Sphere.hpp>

#include <boost/filesystem.hpp>

#include <utility>

YADE_PLUGIN((SimpleShear))
CREATE_LOGGER(SimpleShear);

SimpleShear::~SimpleShear ()
{

}




bool SimpleShear::generate(std::string& message)
{
	scene = shared_ptr<Scene>(new Scene);
	createActors(scene);


// Box walls
	shared_ptr<Body> w1;	// The left one :
	createBox(w1,Vector3r(-thickness/2.0,(height)/2.0,0),Vector3r(thickness/2.0,5*(height/2.0+thickness),width/2.0));
	scene->bodies->insert(w1);


	shared_ptr<Body> w2;	// The lower one :
	createBox(w2,Vector3r(length/2.0,-thickness/2.0,0),Vector3r(length/2.0,thickness/2.0,width/2.0));
	YADE_PTR_CAST<FrictMat> (w2->material)->frictionAngle = matFrictionDeg * Mathr::PI/180.0; // so that we have phi(spheres-inferior wall)=phi(sphere-sphere)
	scene->bodies->insert(w2);

	shared_ptr<Body> w3;	// The right one
	createBox(w3,Vector3r(length+thickness/2.0,height/2.0,0),Vector3r(thickness/2.0,5*(height/2.0+thickness),width/2.0));
	scene->bodies->insert(w3);

	shared_ptr<Body> w4; // The upper one
	createBox(w4,Vector3r(length/2.0,height+thickness/2.0,0),Vector3r(length/2.0,thickness/2.0,width/2.0));
	YADE_PTR_CAST<FrictMat> (w4->material)->frictionAngle = matFrictionDeg * Mathr::PI/180.0; // so that we have phi(spheres-superior wall)=phi(sphere-sphere)
	scene->bodies->insert(w4);

// To close the front and the back of the box 
	shared_ptr<Body> w5;	// behind
	createBox(w5,Vector3r(length/2.0,height/2.0,-width/2.0-thickness/2.0),	Vector3r(2.5*length/2.0,height/2.0+thickness,thickness/2.0));
	scene->bodies->insert(w5);

	shared_ptr<Body> w6;	// the front
	createBox(w6,Vector3r(length/2.0,height/2.0,width/2.0+thickness/2.0),
	Vector3r(2.5*length/2.0,height/2.0+thickness,thickness/2.0));
	scene->bodies->insert(w6);


// the list which will contain the positions of centers and the radii of the created spheres
	vector<BasicSphere> sphere_list;

// to use the TriaxialTest method :
	string out=GenerateCloud(sphere_list,Vector3r(0,0,-width/2.0),Vector3r(length,height,width/2.0),1000,0.3,0.7);// tries to generate a sample of 1000 spheres, with a required porosity of 0.7
	cout << out << endl;

// to use a text file :
// 	std::pair<string,bool> res=ImportCloud(sphere_list,filename);
	
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
	
	shared_ptr<Body> body;
	for (;it!=it_end; ++it)
	{
		createSphere(body,it->first,it->second);
		scene->bodies->insert(body);
	}
	
	return true;
}

void SimpleShear::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius)
{
	body = shared_ptr<Body>(new Body); body->groupMask=1;
	shared_ptr<FrictMat> mat(new FrictMat);
	shared_ptr<Aabb> aabb(new Aabb);
	shared_ptr<Sphere> iSphere(new Sphere);
	
	body->state->pos		=position;
	body->state->ori		=Quaternionr::Identity();
	body->state->vel		=Vector3r(0,0,0);
	body->state->angVel		=Vector3r(0,0,0);

	Real masse			=4.0/3.0*Mathr::PI*radius*radius*radius*density;
	body->state->mass		=masse;
	body->state->inertia		= Vector3r(2.0/5.0*masse*radius*radius,2.0/5.0*masse*radius*radius,2.0/5.0*masse*radius*radius);

	mat->young			= matYoungModulus;
	mat->poisson			= matPoissonRatio;
	mat->frictionAngle		= matFrictionDeg * Mathr::PI/180.0;
	body->material = mat;

	aabb->color		= Vector3r(0,1,0);

	iSphere->radius			= radius;
	iSphere->color		= ((int)(floor(8*position.x()/length)))%2?Vector3r(0.7,0.7,0.7):Vector3r(0.45,0.45,0.45);// so that we have eight different colour bands

	body->shape			= iSphere;
	body->bound			= aabb;
}


void SimpleShear::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body); body->groupMask=1;
	shared_ptr<FrictMat> mat(new FrictMat);
	shared_ptr<Aabb> aabb(new Aabb);
// 	shared_ptr<BoxModel> gBox(new BoxModel);
	shared_ptr<Box> iBox(new Box);
	iBox->wire = true;

	body->setDynamic(false);
	
	body->state->angVel		= Vector3r(0,0,0);
	body->state->vel		= Vector3r(0,0,0);
// 	NB : mass and inertia not defined because not used, since Box are not dynamics
	body->state->pos		= position;
	body->state->ori		= Quaternionr::Identity();

	mat->young	= matYoungModulus;
	mat->poisson	= matPoissonRatio;
	mat->frictionAngle	= 0.0;	//default value, modified after for w2 and w4 to have good values of phi(sphere-walls)
	body->material = mat;

	aabb->color		= Vector3r(1,0,0);

/*	gBox->extents			= extents;
	gBox->color		= Vector3r(1,0,0);
	gBox->wire			= true;
	gBox->shadowCaster		= false;*/
	
	iBox->extents			= extents;
	iBox->color		= Vector3r(1,0,0);

	body->bound			= aabb;
	body->shape			= iBox;
// 	body->geometricalModel		= gBox;
}


void SimpleShear::createActors(shared_ptr<Scene>& scene)
{
	shared_ptr<IGeomDispatcher> interactionGeometryDispatcher(new IGeomDispatcher);
	interactionGeometryDispatcher->add(new Ig2_Sphere_Sphere_ScGeom);
	interactionGeometryDispatcher->add(new Ig2_Box_Sphere_ScGeom);

	shared_ptr<IPhysDispatcher> interactionPhysicsDispatcher(new IPhysDispatcher);
	shared_ptr<IPhysFunctor> CL1Rel(new Ip2_FrictMat_FrictMat_FrictPhys);
	interactionPhysicsDispatcher->add(CL1Rel);

	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
	collider->boundDispatcher->add(new Bo1_Sphere_Aabb);
	collider->boundDispatcher->add(new Bo1_Box_Aabb);
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	

	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt=3e-6;

	shared_ptr<KinemCTDEngine> kinemEngine (new KinemCTDEngine);
	kinemEngine->compSpeed = 10.0;
	kinemEngine->targetSigma=2000.0;


	shared_ptr<InteractionLoop> ids(new InteractionLoop);
	ids->geomDispatcher=interactionGeometryDispatcher;
	ids->physDispatcher=interactionPhysicsDispatcher;
	ids->lawDispatcher=shared_ptr<LawDispatcher>(new LawDispatcher);
	shared_ptr<Law2_ScGeom_FrictPhys_CundallStrack> ldc(new Law2_ScGeom_FrictPhys_CundallStrack);
	ids->lawDispatcher->add(ldc);


	scene->engines.clear();
	scene->engines.push_back(shared_ptr<Engine>(new ForceResetter));
	scene->engines.push_back(globalStiffnessTimeStepper);
	scene->engines.push_back(collider);	
	scene->engines.push_back(ids);
	if(gravApplied)
		scene->engines.push_back(gravityCondition);
	scene->engines.push_back(shared_ptr<Engine> (new NewtonIntegrator));
	scene->engines.push_back(kinemEngine);
}


string SimpleShear::GenerateCloud(vector<BasicSphere>& sphere_list,Vector3r lowerCorner,Vector3r upperCorner,long number,Real rad_std_dev, Real porosity)
{
	sphere_list.clear();
	long tries = 1000; //nb max of tries for positionning the next sphere
	Vector3r dimensions = upperCorner - lowerCorner;
	Real mean_radius = pow(dimensions.x()*dimensions.y()*dimensions.z()*(1-porosity)/(4.0/3.0*Mathr::PI*number),1.0/3.0);
	cerr << " mean radius " << mean_radius << endl;;

// 	std::cerr << "generating aggregates ... ";
	
	long t, i;
	for (i=0; i<number; ++i) 
	{
		BasicSphere s;
		for (t=0; t<tries; ++t) 
		{
			s.second = (Mathr::UnitRandom()-0.5)*rad_std_dev*mean_radius+mean_radius;
			s.first.x() = lowerCorner.x()+s.second+(dimensions.x()-2*s.second)*Mathr::UnitRandom();
			s.first.y() = lowerCorner.y()+s.second+(dimensions.y()-2*s.second)*Mathr::UnitRandom();
			s.first.z() = lowerCorner.z()+s.second+(dimensions.z()-2*s.second)*Mathr::UnitRandom();
			bool overlap=false;
			for (long j=0; (j<i && !overlap); j++)
				if ( pow(sphere_list[j].second+s.second, 2) > (sphere_list[j].first-s.first).squaredNorm()) overlap=true;
			if (!overlap)
			{
				sphere_list.push_back(s);
// 				cout << "j'ai bien rajoute une sphere dans la liste" << endl;
				break;
			}
		}
		if (t==tries) 
		{
		string str1="Generated a sample with " + boost::lexical_cast<string>(i) + " spheres inside box of dimensions: (" 
			+ boost::lexical_cast<string>(dimensions[0]) + "," 
			+ boost::lexical_cast<string>(dimensions[1]) + "," 
			+ boost::lexical_cast<string>(dimensions[2]) + ").\n";
		return str1 + "More than " + boost::lexical_cast<string>(tries) +	" tries while generating sphere number " +
					boost::lexical_cast<string>(i+1) + "/" + boost::lexical_cast<string>(number) + ".";
		}
	}
	return "Generated a sample with " + boost::lexical_cast<string>(number) + " spheres inside box of dimensions: (" 
			+ boost::lexical_cast<string>(dimensions[0]) + "," 
			+ boost::lexical_cast<string>(dimensions[1]) + "," 
			+ boost::lexical_cast<string>(dimensions[2]) + ").";
}

std::pair<string,bool> SimpleShear::ImportCloud(vector<BasicSphere>& sphere_list,string importFilename)
{
	sphere_list.clear();
	int nombre=0;
	if(importFilename.size() != 0 && boost::filesystem::exists(importFilename) )
	{
		ifstream loadFile(importFilename.c_str()); // cree l'objet loadFile de la classe ifstream qui va permettre de lire ce qu'il y a dans importFilename

// 		Real zJF;
// 		while( !loadFile.eof() )	// tant qu'on n'est pas a la fin du fichier
// 		{
// 			BasicSphere s;		// l'elt de la liste sphere_list (= la sphere) que l'on va lire maintenant
// 			loadFile >> s.first.X();// le X de la position de son centre
// 			loadFile >>  zJF;
// 			s.first.Z()=zJF - width/2.0;// le Z de la position de son centre
// 			loadFile >> s.first.Y();// le Y de la position de son centre
// 			loadFile >> s.second;	// son rayon
// 			sphere_list.push_back(s);
// 			nombre++;
// 		}
		Real it;
		while( !loadFile.eof() )	// tant qu'on n'est pas a la fin du fichier
		{
			BasicSphere s;		// l'elt de la liste sphere_list (= la sphere) que l'on va lire maintenant
			loadFile >> it;
			loadFile >> s.second;	// son rayon	
			loadFile >> s.first.x();
			loadFile >> s.first.y();// le y de la position de son centre
			loadFile >> s.first.z();// le z de la position de son centre
			sphere_list.push_back(s);
			nombre++;
		}		
		return std::make_pair(std::string("Echantillon correctement genere : " + boost::lexical_cast<string>(nombre) + " billes"),true);
	}
	else
	{
		cerr << "Cannot find input file" << endl;
		return std::make_pair("Cannot find input file",false);
	}
}




