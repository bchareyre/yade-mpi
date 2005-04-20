
#include "FEMSetTextLoaderFunctor.hpp"
#include "FEMSetParameters.hpp"
#include "FEMNodeData.hpp"
#include "FEMTetrahedronData.hpp"
#include "Sphere.hpp"
#include "Tetrahedron.hpp"
#include "TranslationCondition.hpp"

#include <boost/filesystem/convenience.hpp>

void FEMSetTextLoaderFunctor::registerAttributes()
{
	REGISTER_ATTRIBUTE(fileName);
}

void FEMSetTextLoaderFunctor::go(	  const shared_ptr<BodyPhysicalParameters>& par
					, Body* body)

{
	ComplexBody* rootBody = dynamic_cast<ComplexBody*>(body);
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
		while( ! loadFile.eof() )
			parseCommand(rootBody,loadFile);
	}
	
	for( rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )  // FIXME - this loop should be somewhere in InteractionPhysicsDispatcher
	{
		if(rootBody->bodies->getCurrent()->getGroupMask() & tetrahedronGroupMask)
			dynamic_cast<FEMTetrahedronData*>( rootBody->bodies->getCurrent()->physicalParameters.get() )->calcKeMatrix(rootBody); // FIXME - that should be done inside InteractionPhysicsFunctor
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSetTextLoaderFunctor::parseCommand(       ComplexBody* rootBody
						, ifstream& loadFile)
{ // FIXME - all that stuff should use TXTManager, to deserialize TXT file.
	Vector3r regionMin, regionMax;
	string command;
	loadFile >> command;
	
	if(command == "Region")
	{
		loadFile >> command;
		assert(command == "min");
		loadFile >> regionMin[0] >> regionMin[1] >> regionMin[2];
		
		loadFile >> command;
		assert(command == "max");
		loadFile >> regionMax[0] >> regionMax[1] >> regionMax[2];
		
		loadFile >> command;
		
		if(command == "TranslationCondition")
		{
			Vector3r translationAxis;
			Real velocity;
			
			loadFile >> command;
			assert(command == "velocity" );
			loadFile >> velocity;
			loadFile >> command;
			assert(command == "translationAxis");
			loadFile >> translationAxis[0] >> translationAxis[1] >> translationAxis[2];
			
			shared_ptr<TranslationCondition> translationCondition = shared_ptr<TranslationCondition>(new TranslationCondition);
		 	translationCondition->velocity  = velocity;
			translationAxis.normalize();
		 	translationCondition->translationAxis = translationAxis;
			
			rootBody->actors.push_back(translationCondition);
			translationCondition->subscribedBodies.clear();
			
			for(rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
			{
				if( rootBody->bodies->getCurrent()->getGroupMask() & nodeGroupMask )
				{
					Vector3r pos = rootBody->bodies->getCurrent()->physicalParameters->se3.position;
					if(        pos[0] > regionMin[0] 
						&& pos[1] > regionMin[1] 
						&& pos[2] > regionMin[2] 
						&& pos[0] < regionMax[0] 
						&& pos[1] < regionMax[1] 
						&& pos[2] < regionMax[2] )
					{
						rootBody->bodies->getCurrent()->isDynamic = false;
						rootBody->bodies->getCurrent()->geometricalModel->diffuseColor = Vector3r(1,0,0);
						translationCondition->subscribedBodies.push_back(rootBody->bodies->getCurrent()->getId());
					}
				}
			}
		}
		else
			std::cerr << "Unknown command in FEM file: " << command << "\n";
	} 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSetTextLoaderFunctor::createNode( shared_ptr<Body>& body
					, Vector3r position
					, unsigned int id)
{
	body = shared_ptr<Body>(new SimpleBody(id,nodeGroupMask));
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

void FEMSetTextLoaderFunctor::createTetrahedron(  const ComplexBody* rootBody
						, shared_ptr<Body>& body
						, unsigned int id
						, unsigned int id1
						, unsigned int id2
						, unsigned int id3
						, unsigned int id4)
{
	
	body = shared_ptr<Body>(new SimpleBody(id,tetrahedronGroupMask));
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

