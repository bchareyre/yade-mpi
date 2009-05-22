/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

///
// FIXME : add CohesiveContactLaw engine to make it work
///

#include "HydraulicTest.hpp"

#include<yade/pkg-dem/CohesiveFrictionalContactLaw.hpp>
#include<yade/pkg-dem/CohesiveFrictionalRelationships.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>

#include<yade/core/yadeExceptions.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/DistantPersistentSAPCollider.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>

#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/GravityEngines.hpp>

#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/core/Body.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>

#include<yade/pkg-common/TranslationEngine.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include<yade/pkg-dem/TriaxialCompressionEngine.hpp>



HydraulicTest::HydraulicTest () : FileGenerator()
{
	yadeFileWithSpheres	="./compression1.xml";
	file 			="./spheres_position.dat";
	radius 			=0.7;
}


HydraulicTest::~HydraulicTest ()
{

}


void HydraulicTest::postProcessAttributes(bool)
{
}


void HydraulicTest::registerAttributes()
{
	FileGenerator::registerAttributes();
	
	REGISTER_ATTRIBUTE(yadeFileWithSpheres);
	REGISTER_ATTRIBUTE(file);
	REGISTER_ATTRIBUTE(radius);
	//REGISTER_ATTRIBUTE(outputFileName);
	//REGISTER_ATTRIBUTE(serializationDynlib);
}


bool HydraulicTest::generate()
{
	rootBody = shared_ptr<MetaBody> ( new MetaBody );
	positionRootBody ( rootBody );



/////////////////////////////////////
/////////////////////////////////////
	// load file

	shared_ptr<MetaBody> metaBodyWithSpheres;

	if ( yadeFileWithSpheres.size() !=0
			&& filesystem::exists ( yadeFileWithSpheres )
			&& ( filesystem::extension ( yadeFileWithSpheres ) ==".xml" || filesystem::extension ( yadeFileWithSpheres ) ==".yade" ) )
	{
		try
		{
			if ( filesystem::extension ( yadeFileWithSpheres ) ==".xml" )
				IOFormatManager::loadFromFile ( "XMLFormatManager",yadeFileWithSpheres,"rootBody",metaBodyWithSpheres );

			else if ( filesystem::extension ( yadeFileWithSpheres ) ==".yade" )
				IOFormatManager::loadFromFile ( "BINFormatManager",yadeFileWithSpheres,"rootBody",metaBodyWithSpheres );

		if ( metaBodyWithSpheres->getClassName() != "MetaBody" ) { message="Error: cannot load the file that should contain spheres"; return false; }
		}
		catch ( SerializableError& e )
		{
			message="Error: cannot load the file that should contain spheres"; return false;
		}
		catch ( yadeError& e )
		{
			message="Error: cannot load the file that should contain spheres"; return false;
		}
	}
	else
	{
		message="Error: cannot load the file that should contain spheres";
		return false;
	}
/////////////////////////////////////
	Vector3r min ( 10000,10000,10000 ),max ( -10000,-10000,-10000 );
	{// calc min/max
		BodyContainer::iterator bi    = metaBodyWithSpheres->bodies->begin();
		BodyContainer::iterator biEnd = metaBodyWithSpheres->bodies->end();
		for ( ; bi!=biEnd ; ++bi )
		{
			if ( ( *bi )->geometricalModel->getClassName() == "Sphere" )
			{
				shared_ptr<Body> b = *bi;
				min = componentMinVector ( min,b->physicalParameters->se3.position - static_cast<Sphere*> ( b->geometricalModel.get() )->radius * Vector3r ( 1,1,1 ) );
				max = componentMaxVector ( max,b->physicalParameters->se3.position + static_cast<Sphere*> ( b->geometricalModel.get() )->radius * Vector3r ( 1,1,1 ) );

				BodyMacroParameters* bm = dynamic_cast<BodyMacroParameters*> ( b->physicalParameters.get() );
				if ( !bm ) {message="Error: spheres don't use BodyMacroParameters for physical parameters"; return false;}

			}
		}
	}

	std::cerr << min << " " << max << std::endl;

	unsigned int nbSpheres=0;
	unsigned int idSphere=6;
	{// insert Spheres
		BodyContainer::iterator bi    = metaBodyWithSpheres->bodies->begin();
		BodyContainer::iterator biEnd = metaBodyWithSpheres->bodies->end();
		for ( ; bi!=biEnd ; ++bi )
		{
			if ( ( *bi )->geometricalModel->getClassName() == "Sphere" )
			{
				if ( inside ( ( *bi )->physicalParameters->se3.position ) )
				{
					cerr << "insert sphere" << endl;
					shared_ptr<Body> b = *bi;
					//b->id = idSphere++;
					rootBody->bodies->insert ( b, idSphere++ );
					nbSpheres++;
				}
			}
			if ( ( *bi )->geometricalModel->getClassName() == "Box" )
			{
				//if(  inside((*bi)->physicalParameters->se3.position)  )
				{
					cerr << "insert box" << endl;
					shared_ptr<Body> b = *bi;
					rootBody->bodies->insert ( b );

				}
			}
		}
	}

// saving file
	if ( file.size() != 0 )
	{
		ofstream saveFile ( file.c_str() );
		//saveFile << nbSpheres << std::endl;

		//BodyContainer::iterator bi    = metaBodyWithSpheres->bodies->begin();
		//BodyContainer::iterator biEnd = metaBodyWithSpheres->bodies->end();
		BodyContainer::iterator bi    = rootBody->bodies->begin();
		BodyContainer::iterator biEnd = rootBody->bodies->end();
		for ( ; bi!=biEnd ; ++bi )
		{
			if ( ( *bi )->geometricalModel->getClassName() == "Sphere" )
			{
				saveFile << ( *bi )->getId() << " " << YADE_PTR_CAST<Sphere> ( ( *bi )->geometricalModel )->radius  << " " << ( *bi )->physicalParameters->se3.position << std::endl;

			}
		}
		saveFile.close();
	}

/////////////////////////////////////

//  rootBody->persistentInteractions->clear();
//
//  shared_ptr<Body> bodyA;
//
//  BodyContainer::iterator bi    = rootBody->bodies->begin();
//  BodyContainer::iterator biEnd = rootBody->bodies->end();
//  BodyContainer::iterator bi2;
//
//  ++bi; // skips piston
//  ++bi; // skips supportBox1
//  ++bi; // skips supportBox2
//

//  for( ; bi!=biEnd ; ++bi )
//  {
//   bodyA =*bi;
//   bi2=bi;
//   ++bi2;
//   for( ; bi2!=biEnd ; ++bi2 )
//   {
//    if(   (*bi)->geometricalModel->getClassName() == "Sphere"
//       && (*bi2)->geometricalModel->getClassName() == "Sphere")
//    {
//    shared_ptr<Body> bodyB = *bi2;
//
//    shared_ptr<BodyMacroParameters> a = YADE_PTR_CAST<BodyMacroParameters>(bodyA->physicalParameters);
//    shared_ptr<BodyMacroParameters> b = YADE_PTR_CAST<BodyMacroParameters>(bodyB->physicalParameters);
//    shared_ptr<InteractingSphere> as = YADE_PTR_CAST<InteractingSphere>(bodyA->interactingGeometry);
//    shared_ptr<InteractingSphere> bs = YADE_PTR_CAST<InteractingSphere>(bodyB->interactingGeometry);
//
// /*   if ((a->se3.position - b->se3.position).Length() < (as->radius + bs->radius))
//    {
//     shared_ptr<Interaction>   link(new Interaction( bodyA->getId() , bodyB->getId() ));
//     shared_ptr<SDECLinkGeometry>  geometry(new SDECLinkGeometry);
//     shared_ptr<SDECLinkPhysics> physics(new SDECLinkPhysics);
//
//     geometry->radius1   = as->radius - fabs(as->radius - bs->radius)*0.5;
//     geometry->radius2   = bs->radius - fabs(as->radius - bs->radius)*0.5;
//
//     physics->initialKn   = linkKn; // FIXME - BIG problem here.
//     physics->initialKs   = linkKs;
//     physics->heta    = 1;
//     physics->initialEquilibriumDistance = (a->se3.position - b->se3.position).Length();
//     physics->knMax    = linkMaxNormalForce;
//     physics->ksMax    = linkMaxShearForce;
//
//     link->interactionGeometry   = geometry;
//     link->interactionPhysics   = physics;
//     link->isReal     = true;
//     link->isNew     = false;
//
//     rootBody->persistentInteractions->insert(link);
//    }*/
//    }
//   }
//  }

	/* message="total number of permament links created: "
	  + lexical_cast<string>(rootBody->persistentInteractions->size())
	  + "\nWARNING: link bonds are nearly working, but the formulas are waiting for total rewrite!";
	*/
	rootBody->engines= metaBodyWithSpheres->engines; //Andrea put his dirty hands here!!
	rootBody->initializers= metaBodyWithSpheres->initializers; //Bruno as well

	vector<shared_ptr<Engine> >::iterator it =  rootBody->engines.begin();
	vector<shared_ptr<Engine> >::iterator it_end =  rootBody->engines.end();
	for ( ;it!=it_end; ++it )
	{
		if ( ( *it )->getClassName() == "GravityEngine" )
		{
			cerr << "found gravity engine" << endl;
			YADE_PTR_CAST<GravityEngine> ( *it )->gravity = Vector3r ( 0,-9.81,0 );
		}
		else if ( ( *it )->getClassName() == "TriaxialCompressionEngine" )
		{
			cerr << "found TriaxialCompressionEngine" << endl;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_bottom_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_top_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_left_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_right_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_front_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->wall_back_activated = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->autoCompressionActivation = 0;
			YADE_PTR_CAST<TriaxialCompressionEngine> ( *it )->internalCompaction = 0;

		}
	}
	return true;
}

void HydraulicTest::positionRootBody(shared_ptr<MetaBody>& rootBody)
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

bool HydraulicTest::inside(Vector3r pos)
{
	if(std::sqrt(pow(pos[0]-0.015, 2) + pow(pos[2], 2)) < radius)
		return false;

	return true;
};

YADE_PLUGIN();
