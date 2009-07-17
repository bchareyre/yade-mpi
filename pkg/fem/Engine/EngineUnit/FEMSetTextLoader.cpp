/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMSetTextLoader.hpp"
#include<yade/pkg-fem/FEMSetParameters.hpp>
#include<yade/pkg-fem/FEMNodeData.hpp>
#include<yade/pkg-fem/FEMTetrahedronData.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Tetrahedron.hpp>
#include <boost/filesystem/convenience.hpp>



void FEMSetTextLoader::go(	  const shared_ptr<PhysicalParameters>& par
					, Body* body)

{
	MetaBody* rootBody = YADE_CAST<MetaBody*>(body);
	shared_ptr<FEMSetParameters> physics = YADE_PTR_CAST<FEMSetParameters>(par);
	nodeGroupMask = physics->nodeGroupMask;
	tetrahedronGroupMask = physics->tetrahedronGroupMask;

///////// load file 
	if(fileName.size() != 0 && filesystem::exists(fileName) )
	{
		ifstream loadFile(fileName.c_str());
		unsigned int nbNodes,nbTetrahedrons;
		loadFile >> nbNodes;
		loadFile >> nbTetrahedrons;
		for(unsigned int i = 0 ; i<nbNodes ; ++i)
		{
			Real x,y,z;
			unsigned int id;
			loadFile >> id >> x >> y >> z;
//			cerr << id << " \t" << x << " \t" << y << " \t" << z << endl;
			shared_ptr<Body> femNode;
			createNode(femNode,Vector3r(x,y,z),id);
			rootBody->bodies->insert(femNode);
		}
		for(unsigned int i = 0 ; i<nbTetrahedrons ; ++i)
		{
			unsigned int id,id1,id2,id3,id4;
			loadFile >> id >> id1 >> id2 >> id3 >> id4;
			shared_ptr<Body> femTetrahedron;
			createTetrahedron(rootBody,femTetrahedron,id,id1,id2,id3,id4);
			rootBody->bodies->insert(femTetrahedron);
		}
	}
	
}



void FEMSetTextLoader::createNode( shared_ptr<Body>& body
					, Vector3r position
					, int id)
{
	body = shared_ptr<Body>(new Body(body_id_t(id),nodeGroupMask));
	shared_ptr<FEMNodeData> physics(new FEMNodeData);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1) , 0 );
	
	Real radius 			= 0.5;
	
	body->isDynamic			= true;
//cerr << position << endl;	
//	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->acceleration		= Vector3r(0,0,0);
	physics->mass			= 1;
//	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(position,q);
	physics->initialPosition	= position;

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3r(0.9,0.9,0.3);
	gSphere->wire			= false;
	gSphere->shadowCaster		= true;
	
	body->geometricalModel		= gSphere;
	body->physicalParameters	= physics;
}


void FEMSetTextLoader::createTetrahedron(  const MetaBody* rootBody
						, shared_ptr<Body>& body
						, int id
						, int id1
						, int id2
						, int id3
						, int id4)
{
	
	body = shared_ptr<Body>(new Body(body_id_t(id),tetrahedronGroupMask));
	shared_ptr<FEMTetrahedronData> physics(new FEMTetrahedronData);
	shared_ptr<Tetrahedron> gTet(new Tetrahedron);
	
	body->isDynamic			= false;
	
//	physics->angularVelocity	= Vector3r(0,0,0);
//	physics->velocity		= Vector3r(0,0,0);
//	physics->mass			= 1; // FIXME
//	physics->inertia		= Vector3r(1,1,1); // FIXME
	physics->ids.clear();
	physics->ids.push_back(id1);
	physics->ids.push_back(id2);
	physics->ids.push_back(id3);
	physics->ids.push_back(id4);
	// FIXME - remove Se3 - FEMTetrahedron DOES NOT need Se3 !
	physics->se3 			= Se3r( Vector3r(0,0,0) , Quaternionr(0,0,0,0) );

	gTet->diffuseColor		= Vector3r(1,1,1);
	gTet->wire			= false;
	gTet->shadowCaster		= true;
//	gTet->v1 			= (*(rootBody->bodies))[id1]->physicalParameters->se3.position;
//	gTet->v2 			= (*(rootBody->bodies))[id2]->physicalParameters->se3.position;
//	gTet->v3 			= (*(rootBody->bodies))[id3]->physicalParameters->se3.position;
//	gTet->v4 			= (*(rootBody->bodies))[id4]->physicalParameters->se3.position;
	
	body->geometricalModel		= gTet;
	body->physicalParameters	= physics;
}

YADE_PLUGIN();
