
#include "FEMSetTextLoaderFunctor.hpp"

#include "FEMSetParameters.hpp"
#include "FEMNodeData.hpp"
#include "FEMTetrahedronData.hpp"
#include "Sphere.hpp"
#include "Tetrahedron.hpp"

#include <boost/filesystem/convenience.hpp>

void FEMSetTextLoaderFunctor::registerAttributes()
{
	REGISTER_ATTRIBUTE(fileName);
}

void FEMSetTextLoaderFunctor::go(	  const shared_ptr<BodyPhysicalParameters>& par
					, Body* body)

{
	MetaBody* rootBody = dynamic_cast<MetaBody*>(body);
	shared_ptr<FEMSetParameters> physics = dynamic_pointer_cast<FEMSetParameters>(par);
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSetTextLoaderFunctor::createNode( shared_ptr<Body>& body
					, Vector3r position
					, unsigned int id)
{
	body = shared_ptr<Body>(new Body(id,nodeGroupMask));
	shared_ptr<FEMNodeData> physics(new FEMNodeData);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1) , 0 );
	
	Real radius 			= 0.5;
	
	body->isDynamic			= true;
//cerr << position << endl;	
//	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->acceleration		= Vector3r(0,0,0);
	physics->mass			= 1;
	physics->invMass		= 1;
//	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(position,q);
	physics->initialPosition	= position;

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.9,0.9,0.3);
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	body->geometricalModel		= gSphere;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSetTextLoaderFunctor::createTetrahedron(  const MetaBody* rootBody
						, shared_ptr<Body>& body
						, unsigned int id
						, unsigned int id1
						, unsigned int id2
						, unsigned int id3
						, unsigned int id4)
{
	
	body = shared_ptr<Body>(new Body(id,tetrahedronGroupMask));
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

	gTet->diffuseColor		= Vector3f(1,1,1);
	gTet->wire			= false;
	gTet->visible			= true;
	gTet->shadowCaster		= true;
//	gTet->v1 			= (*(rootBody->bodies))[id1]->physicalParameters->se3.position;
//	gTet->v2 			= (*(rootBody->bodies))[id2]->physicalParameters->se3.position;
//	gTet->v3 			= (*(rootBody->bodies))[id3]->physicalParameters->se3.position;
//	gTet->v4 			= (*(rootBody->bodies))[id4]->physicalParameters->se3.position;
	
	body->geometricalModel		= gTet;
	body->physicalParameters	= physics;
}

