/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "SimpleShear.hpp"

#include <yade/lib-miniWm3/Wm3Math.h>

#include <yade/pkg-dem/CohesiveFrictionalMat.hpp>
#include <yade/pkg-dem/ContactLaw1.hpp>
#include <yade/pkg-dem/CL1Relationships.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include <yade/pkg-dem/PositionOrientationRecorder.hpp>

#include <yade/pkg-dem/PositionRecorder.hpp>
#include <yade/pkg-dem/PositionSnapshot.hpp>
#include <yade/pkg-dem/ForceRecorder.hpp>
#include <yade/pkg-dem/ForceSnapshot.hpp>

#include<yade/pkg-common/BoxModel.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/SphereModel.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundDispatcher.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>

#include<yade/pkg-dem/NewtonIntegrator.hpp>
#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-common/CinemDNCEngine.hpp>


#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/StateMetaEngine.hpp>

#include <boost/filesystem/convenience.hpp>
#include <utility>

using namespace std;
YADE_REQUIRE_FEATURE(geometricalmodel);

SimpleShear::SimpleShear () : FileGenerator()
{
	gravity = Vector3r(0,-9.81,0);

// Fichier contenant la liste des positions des centres et des rayons des spheres qui vont generer l'echantillon :
	filename = "../porosite0_44.txt";
//  Deux parametres necessaires si on veut utiliser au contraire la methode de TriaxialTest pour generer l'echantillon de spheres :
// 	porosite=0.75;
// 	nBilles=8000;
	thickness=0.001;
	width=0.1;
	height=0.02;
	profondeur=0.04;

	density=2600;
	sphereYoungModulus=2.0e9; // 2 GPa, nécessaire apparemment si on veut reproduire un joint rocheux
	spherePoissonRatio=0.04; // Venant de "Calibration procedure...." (Plassiard et al)
	sphereFrictionDeg=26;

	boxYoungModulus=2.0e9; // 2 GPa, nécessaire apparemment si on veut reproduire un joint rocheux
	boxPoissonRatio=0.04;

	shearSpeed=0.1;
	
	timeStepUpdateInterval = 50;
	
	gravApplied = true;
	shearApplied = false;

}


SimpleShear::~SimpleShear ()
{

}


void SimpleShear::postProcessAttributes(bool)
{

}




bool SimpleShear::generate()
{
	rootBody = shared_ptr<Scene>(new Scene);
	createActors(rootBody);
	positionRootBody(rootBody);


// BoxModel walls
	shared_ptr<Body> w1;	// The left one :
	createBox(w1,Vector3r(-thickness/2.0,(height)/2.0,0),Vector3r(thickness/2.0,5*(height/2.0+thickness),profondeur/2.0));
	rootBody->bodies->insert(w1);


	shared_ptr<Body> w2;	// The lower one :
	createBox(w2,Vector3r(width/2.0,-thickness/2.0,0),Vector3r(width/2.0,thickness/2.0,profondeur/2.0));
	YADE_CAST<CohesiveFrictionalMat*>(w2->physicalParameters.get())->frictionAngle = sphereFrictionDeg * Mathr::PI/180.0;; // so that we have phi(spheres-inferior wall)=phi(sphere-sphere)
	rootBody->bodies->insert(w2);

	shared_ptr<Body> w3;	// The right one
	createBox(w3,Vector3r(width+thickness/2.0,height/2.0,0),Vector3r(thickness/2.0,5*(height/2.0+thickness),profondeur/2.0));
	rootBody->bodies->insert(w3);

	shared_ptr<Body> w4; // The upper one
	createBox(w4,Vector3r(width/2.0,height+thickness/2.0,0),Vector3r(width/2.0,thickness/2.0,profondeur/2.0));
	YADE_CAST<CohesiveFrictionalMat*>(w4->physicalParameters.get())->frictionAngle = sphereFrictionDeg * Mathr::PI/180.0;; // so that we have phi(spheres-superior wall)=phi(sphere-sphere)
	rootBody->bodies->insert(w4);

// To close the front and the bottom of the box 
	shared_ptr<Body> w5;
	createBox(w5,Vector3r(width/2.0,height/2.0,-profondeur/2.0-thickness/2.0),	Vector3r(2.5*width/2.0,height/2.0+thickness,thickness/2.0));
	rootBody->bodies->insert(w5);

	shared_ptr<Body> w6;
	createBox(w6,Vector3r(width/2.0,height/2.0,profondeur/2.0+thickness/2.0),
	Vector3r(2.5*width/2.0,height/2.0+thickness,thickness/2.0));
	rootBody->bodies->insert(w6);


// the list which will contain the positions of centers and the radii of the created spheres
	vector<BasicSphere> sphere_list;

// to use the TriaxialTest method :
// 	GenerateCloud(sphere_list,Vector3r(0,0,-profondeur/2.0),Vector3r(width,height,profondeur/2.0),nBilles,0.3,porosite);

// to use a text file :
	std::pair<string,bool> res=ImportCloud(sphere_list,filename);
	
	vector<BasicSphere>::iterator it = sphere_list.begin();
	vector<BasicSphere>::iterator it_end = sphere_list.end();
	
	shared_ptr<Body> body;
	for (;it!=it_end; ++it)
	{
		createSphere(body,it->first,it->second);
		rootBody->bodies->insert(body);
	}
	
	message =res.first;
	return res.second;
}

void SimpleShear::createSphere(shared_ptr<Body>& body, Vector3r position, Real radius)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<CohesiveFrictionalMat> physics(new CohesiveFrictionalMat);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<SphereModel> gSphere(new SphereModel);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;
	physics->isCohesive		= 1;

	aabb->diffuseColor		= Vector3r(0,1,0);


	gSphere->radius			= radius;
	// de quoi avoir des bandes (huit en largeur) de couleur differentes :
	gSphere->diffuseColor		= ((int)(Mathr::Floor(8*position.X()/width)))%2?Vector3r(0.7,0.7,0.7):Vector3r(0.45,0.45,0.45);
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3r(0.8,0.3,0.3);

	body->shape	= iSphere;
	body->geometricalModel		= gSphere;
	body->bound		= aabb;
	body->physicalParameters	= physics;
}


void SimpleShear::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<CohesiveFrictionalMat> physics(new CohesiveFrictionalMat);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<BoxModel> gBox(new BoxModel);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	//physics->mass			= 0;
	//physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= boxYoungModulus;
	physics->poisson		= boxPoissonRatio;
	physics->frictionAngle		= 0.0;	//default value, modified after for w2 and w4 to have good values of phi(sphere-walls)
	physics->isCohesive		= 1;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3r(1,0,0);
	gBox->wire			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3r(1,0,0);

	body->bound		= aabb;
	body->shape	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}


void SimpleShear::createActors(shared_ptr<Scene>& rootBody)
{

	shared_ptr<PositionSnapshot> possnap = shared_ptr<PositionSnapshot>(new PositionSnapshot);
	possnap->list_id.clear();
	possnap->list_id.push_back(5);
	possnap->list_id.push_back(10);
	possnap->outputFile="../data/PosSnapshot";

	shared_ptr<ForceSnapshot> forcesnap = shared_ptr<ForceSnapshot>(new ForceSnapshot);
	forcesnap->list_id.clear();
	forcesnap->list_id.push_back(5);
	forcesnap->list_id.push_back(10);
	forcesnap->outputFile="../data/ForceSnapshot";

	shared_ptr<CinemDNCEngine> kinematic = shared_ptr<CinemDNCEngine>(new CinemDNCEngine);
	kinematic->shearSpeed  = shearSpeed;

	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	shared_ptr<InteractionPhysicsFunctor> CL1Rel(new CL1Relationships);
	interactionPhysicsDispatcher->add(CL1Rel);

	shared_ptr<BoundDispatcher> boundDispatcher	= shared_ptr<BoundDispatcher>(new BoundDispatcher);
	boundDispatcher->add("InteractingSphere2AABB");
	boundDispatcher->add("InteractingBox2AABB");
	boundDispatcher->add("MetaInteractingGeometry2AABB");
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	

	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask = 1;
	globalStiffnessTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;
	globalStiffnessTimeStepper->defaultDt=1e-5;




	rootBody->engines.clear();
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(globalStiffnessTimeStepper);
	rootBody->engines.push_back(boundDispatcher);	
	rootBody->engines.push_back(shared_ptr<Engine>(new InsertionSortCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new ContactLaw1));
	if(gravApplied)
		rootBody->engines.push_back(gravityCondition);
	if(shearApplied)
		rootBody->engines.push_back(kinematic);
	rootBody->engines.push_back(shared_ptr<Engine> (new NewtonIntegrator));
// 	rootBody->engines.push_back(possnap);
// 	rootBody->engines.push_back(forcesnap);
	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundDispatcher);
}


void SimpleShear::positionRootBody(shared_ptr<Scene>& rootBody) 
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r(0,0,0);
	physics->acceleration			= Vector3r::ZERO;
		
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3r(0,0,1);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->shape		= YADE_PTR_CAST<Shape>(set);	
	rootBody->bound		= YADE_PTR_CAST<Bound>(aabb);
	rootBody->physicalParameters 		= physics;
}


string SimpleShear::GenerateCloud(vector<BasicSphere>& sphere_list,Vector3r lowerCorner,Vector3r upperCorner,long number,Real rad_std_dev, Real porosity)
{
	sphere_list.clear();
	long tries = 1000; //nb max of tries for positionning the next sphere
	Vector3r dimensions = upperCorner - lowerCorner;
	Real mean_radius = Mathr::Pow(dimensions.X()*dimensions.Y()*dimensions.Z()*(1-porosity)/(4.0/3.0*Mathr::PI*number),1.0/3.0);
	cerr << " mean radius " << mean_radius << endl;;

// 	std::cerr << "generating aggregates ... ";
	
	long t, i;
	for (i=0; i<number; ++i) 
	{
		BasicSphere s;
		for (t=0; t<tries; ++t) 
		{
			s.second = (Mathr::UnitRandom()-0.5)*rad_std_dev*mean_radius+mean_radius;
			s.first.X() = lowerCorner.X()+s.second+(dimensions.X()-2*s.second)*Mathr::UnitRandom();
			s.first.Y() = lowerCorner.Y()+s.second+(dimensions.Y()-2*s.second)*Mathr::UnitRandom();
			s.first.Z() = lowerCorner.Z()+s.second+(dimensions.Z()-2*s.second)*Mathr::UnitRandom();
			bool overlap=false;
			for (long j=0; (j<i && !overlap); j++)
				if ( pow(sphere_list[j].second+s.second, 2) > (sphere_list[j].first-s.first).SquaredLength()) overlap=true;
			if (!overlap)
			{
				sphere_list.push_back(s);
				cout << "j'ai bien rajoute une sphere dans la liste" << endl;
				break;
			}
		}
		if (t==tries) 
		{cerr << "on a atteint le t=tries" << endl;
		return "More than " + lexical_cast<string>(tries) +	" tries while generating sphere number " +
					lexical_cast<string>(i+1) + "/" + lexical_cast<string>(number) + ".";
		}
	}
	return "Generated a sample with " + lexical_cast<string>(number) + "spheres inside box of dimensions: (" 
			+ lexical_cast<string>(dimensions[0]) + "," 
			+ lexical_cast<string>(dimensions[1]) + "," 
			+ lexical_cast<string>(dimensions[2]) + ").";
}

std::pair<string,bool> SimpleShear::ImportCloud(vector<BasicSphere>& sphere_list,string importFilename)
{
	sphere_list.clear();
	int nombre=0;
	if(importFilename.size() != 0 && filesystem::exists(importFilename) )
	{
		ifstream loadFile(importFilename.c_str()); // cree l'objet loadFile de la classe ifstream qui va permettre de lire ce qu'il y a dans importFilename
		Real zJF;
		while( !loadFile.eof() )	// tant qu'on n'est pas a la fin du fichier
		{
			BasicSphere s;		// l'elt de la liste sphere_list (= la sphere) que l'on va lire maintenant
			loadFile >> s.first.X();// le X de la position de son centre
			loadFile >>  zJF;
			s.first.Z()=zJF - profondeur/2.0;// le Z de la position de son centre
			loadFile >> s.first.Y();// le Y de la position de son centre
			loadFile >> s.second;	// son rayon
			sphere_list.push_back(s);
			nombre++;
		}
		return std::make_pair(std::string("Echantillon correctement genere : " + lexical_cast<string>(nombre) + " billes"),true);
	}
	else
	{
		cerr << "Cannot find input file" << endl;
		return std::make_pair("Cannot find input file",false);
	}
}



YADE_REQUIRE_FEATURE(PHYSPAR);

