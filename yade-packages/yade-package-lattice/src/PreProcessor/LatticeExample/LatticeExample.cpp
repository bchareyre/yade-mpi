/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeExample.hpp"

#include "LatticeSetParameters.hpp"
#include "LatticeBeamParameters.hpp"
#include "LatticeBeamAngularSpring.hpp"
#include "LatticeNodeParameters.hpp"
#include "LineSegment.hpp"
#include "LatticeLaw.hpp"

#include <yade/yade-package-common/Sphere.hpp>

#include <yade/yade-core/Body.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry.hpp>
#include <yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-package-common/GeometricalModelMetaEngine.hpp>

#include <yade/yade-package-common/AABB.hpp>

#include <yade/yade-package-common/BodyRedirectionVector.hpp>
#include <yade/yade-package-common/InteractionVecSet.hpp>
#include <yade/yade-package-common/PhysicalActionVectorVector.hpp>

#include <yade/yade-package-common/DisplacementEngine.hpp>
#include <yade/yade-package-common/PhysicalParametersMetaEngine.hpp>
#include <yade/yade-package-common/PhysicalActionApplier.hpp>

#include <yade/yade-package-common/PhysicalActionContainerInitializer.hpp>


using namespace boost;
using namespace std;


LatticeExample::LatticeExample() : FileGenerator()
{
	nodeGroupMask 		= 1;
	beamGroupMask 		= 2;
	
	speciemen_size_in_meters = Vector3r(0.1,0.1,0.0001);
	cellsizeUnit_in_meters	 = 0.003;
	minAngle_betweenBeams_deg= 20.0;
	disorder_in_cellsizeUnit = Vector3r(0.6,0.6,0.0);
	maxLength_in_cellsizeUnit= 1.9;
	triangularBaseGrid 	 = true;
				
	crit_TensileStrain_percent = 100.0;	// E_min
	crit_ComprStrain_percent   = 50.0;	// E_max
			
	longitudalStiffness_noUnit = 1.0;	// k_l
	bendingStiffness_noUnit    = 0.0;	// k_b
	
	region_A_min 		= Vector3r(-0.006, 0.096,-1);
	region_A_max 		= Vector3r( 0.16 , 0.16 , 1);
	direction_A 		= Vector3r(0,1,0);
	displacement_A_meters	= 0.0001;
	
	region_B_min 		= Vector3r(-0.006,-0.006,-1);
	region_B_max 		= Vector3r( 0.16 , 0.004, 1);
	direction_B 		= Vector3r(0,-1,0);
	displacement_B_meters	= 0.0001;

	region_C_min 		= Vector3r(-0.006, 0.096,-1);
	region_C_max 		= Vector3r( 0.16 , 0.16 , 1);
	direction_C 		= Vector3r(0,1,0);
	displacement_C_meters	= 0.0001;
	
	region_D_min 		= Vector3r(-0.006,-0.006,-1);
	region_D_max 		= Vector3r( 0.16 , 0.004, 1);
	direction_D 		= Vector3r(0,-1,0);
	displacement_D_meters	= 0.0001;
	
}


LatticeExample::~LatticeExample()
{

}


void LatticeExample::registerAttributes()
{
	REGISTER_ATTRIBUTE(speciemen_size_in_meters); 	// size
	REGISTER_ATTRIBUTE(cellsizeUnit_in_meters);	// g [m]  	- cell size
	REGISTER_ATTRIBUTE(minAngle_betweenBeams_deg); 	// a [deg] 	- min angle
	REGISTER_ATTRIBUTE(disorder_in_cellsizeUnit); 	// s [-] 	- disorder 
	REGISTER_ATTRIBUTE(maxLength_in_cellsizeUnit);	// r [-] 	- max beam length
	
	REGISTER_ATTRIBUTE(crit_TensileStrain_percent); // E_min [%]    - default 0.02 %
	REGISTER_ATTRIBUTE(crit_ComprStrain_percent);   // E_max [%]    - default 0.2 %
	REGISTER_ATTRIBUTE(longitudalStiffness_noUnit); // k_l [-]      - default 1.0
	REGISTER_ATTRIBUTE(bendingStiffness_noUnit);    // k_b [-]      - default 0.6
	
	REGISTER_ATTRIBUTE(triangularBaseGrid); 	// 		- triangles
	
	REGISTER_ATTRIBUTE(region_A_min);
	REGISTER_ATTRIBUTE(region_A_max);
	REGISTER_ATTRIBUTE(direction_A);
	REGISTER_ATTRIBUTE(displacement_A_meters);
	
	REGISTER_ATTRIBUTE(region_B_min);
	REGISTER_ATTRIBUTE(region_B_max);
	REGISTER_ATTRIBUTE(direction_B);
	REGISTER_ATTRIBUTE(displacement_B_meters);
	
	REGISTER_ATTRIBUTE(region_C_min);
	REGISTER_ATTRIBUTE(region_C_max);
	REGISTER_ATTRIBUTE(direction_C);
	REGISTER_ATTRIBUTE(displacement_C_meters);
	
	REGISTER_ATTRIBUTE(region_D_min);
	REGISTER_ATTRIBUTE(region_D_max);
	REGISTER_ATTRIBUTE(direction_D);
	REGISTER_ATTRIBUTE(displacement_D_meters);
}

string LatticeExample::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	
	shared_ptr<Body> body;
	
	Vector3r nbNodes = speciemen_size_in_meters / cellsizeUnit_in_meters;
	if(triangularBaseGrid)
		nbNodes[1] *= 1.15471; // bigger by sqrt(3)/2 factor

	unsigned int totalNodesCount = 0;

	for( int i=0 ; i<=nbNodes[0] ; i++ )
		for( int j=0 ; j<=nbNodes[1] ; j++ )
			for( int k=0 ; k<=nbNodes[2] ; k++)
			{
				shared_ptr<Body> node;
				if(createNode(node,i,j,k))
					rootBody->bodies->insert(node), ++totalNodesCount;
			}

	BodyRedirectionVector bc;
	bc.clear();

	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator bi2;
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	int beam_counter = 0;
	float nodes_a=0;
	float nodes_all = rootBody->bodies->size();
	for(  ; bi!=biEnd ; ++bi )  // loop over all nodes, to create beams
	{
		Body* bodyA = (*bi).get(); // first_node
	
		bi2 = bi;
		++bi2;
		nodes_a+=1.0;
		
		for( ; bi2!=biEnd ; ++bi2 )
		{
			Body* bodyB = (*bi2).get(); // all other nodes
			// warning - I'm assuming that there are ONLY Nodes in the rootBody
			LatticeNodeParameters* a = static_cast<LatticeNodeParameters*>(bodyA->physicalParameters.get());
			LatticeNodeParameters* b = static_cast<LatticeNodeParameters*>(bodyB->physicalParameters.get());
			
			if ((a->se3.position - b->se3.position).squaredLength() < std::pow(maxLength_in_cellsizeUnit*cellsizeUnit_in_meters,2) )  
			{
				shared_ptr<Body> beam;
				createBeam(beam,bodyA->getId(),bodyB->getId());
				calcBeamPositionOrientationLength(beam);
				if(checkMinimumAngle(bc,beam))
				{
					if( ++beam_counter % 100 == 0 )
						cerr << "creating beam: " << beam_counter << " , " << ((nodes_a/nodes_all)*100.0)  << " %\n"; 
					
					bc.insert(beam);
				}
			}
		}
	}

	bi    = bc.begin();
	biEnd = bc.end();
	for(  ; bi!=biEnd ; ++bi )  // loop over all newly created beams ...
	{
		shared_ptr<Body> b = *bi;
		rootBody->bodies->insert(b); // .. to insert then into rootBody
	}

	{ // remember what node is in contact with what beams
		//    node                   beams
		connections.resize(totalNodesCount);
		
		bi    = rootBody->bodies->begin();
		biEnd = rootBody->bodies->end();
		
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			Body* body = (*bi).get();
			if( ! ( body->getGroupMask() & beamGroupMask ) )
				continue; // skip non-beams
			
			LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );
			connections[beam->id1].push_back(body->getId());
			connections[beam->id2].push_back(body->getId());
		}
	}
	
	{ // create angular springs between beams
		bi    = rootBody->bodies->begin();
		biEnd = rootBody->bodies->end();
		float all_bodies = rootBody->bodies->size();
		int current = 0;
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			if( ++current % 100 == 0 )
				cerr << "making angular springs: " << current << " , " << ((static_cast<float>(current)/all_bodies)*100.0) << " %\n";
				
			Body* body = (*bi).get();
			if( ! ( body->getGroupMask() & beamGroupMask ) )
				continue; // skip non-beams
				
			calcBeamAngles(body,rootBody->bodies.get(),rootBody->persistentInteractions.get());
		}
	}
	
	imposeTranslation(rootBody,region_A_min,region_A_max,direction_A,displacement_A_meters);
	imposeTranslation(rootBody,region_B_min,region_B_max,direction_B,displacement_B_meters);
	imposeTranslation(rootBody,region_C_min,region_C_max,direction_C,displacement_C_meters);
	imposeTranslation(rootBody,region_D_min,region_D_max,direction_D,displacement_D_meters);

	cerr << "finished.. saving\n";

 	return "Number of nodes created:\n" + lexical_cast<string>(nbNodes[0]) + ","
	 				    + lexical_cast<string>(nbNodes[1]) + ","
					    + lexical_cast<string>(nbNodes[2]);

}

/// returns true if angle is bigger than minAngle_betweenBeams_deg
bool LatticeExample::checkAngle( Vector3r a, Vector3r& b)
{
	Quaternionr al;
	al.align(a,b);
	Vector3r axis;
	Real angle;
	al.toAxisAngle(axis, angle);
	angle *= 180.0/Mathr::PI ;
//	cerr << " angle: " << angle << "\n";
	return angle > minAngle_betweenBeams_deg;
}

/// returns true if angle is bigger than minAngle_betweenBeams_deg
bool LatticeExample::checkMinimumAngle(BodyRedirectionVector& bc,shared_ptr<Body>& body)
{
	bool answer = true;
	LatticeBeamParameters* newBeam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get());
	
	BodyContainer::iterator bi    = bc.begin();
	BodyContainer::iterator biEnd = bc.end();
	for(  ; bi!=biEnd ; ++bi )  // loop over all beams - they MUST be beams, for static_cast<> 
	{ 
		LatticeBeamParameters* oldBeam = static_cast<LatticeBeamParameters*>((*bi)->physicalParameters.get());
		if( 	   (oldBeam->id1 == newBeam->id1)
			|| (oldBeam->id2 == newBeam->id2))
			answer = answer && checkAngle(   oldBeam->direction ,  newBeam->direction );
		if( 	   (oldBeam->id2 == newBeam->id1)
			|| (oldBeam->id1 == newBeam->id2))
			answer = answer && checkAngle( - oldBeam->direction ,  newBeam->direction );
	} 
	return answer;
}

bool LatticeExample::createNode(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,nodeGroupMask));
	shared_ptr<LatticeNodeParameters> physics(new LatticeNodeParameters);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(Mathr::unitRandom(),Mathr::unitRandom(),Mathr::unitRandom()) , Mathr::unitRandom()*Mathr::PI );
	
	float  triang_x = triangularBaseGrid ? (static_cast<float>(j%2))*0.5 : 0;
	double triang_y = triangularBaseGrid ? 0.86602540378443864676        : 1; // sqrt(3)/2
	
	Vector3r position		= ( Vector3r(i+triang_x,j*triang_y,k)
					  + Vector3r( 	  Mathr::symmetricRandom()*disorder_in_cellsizeUnit[0]
					  		, Mathr::symmetricRandom()*disorder_in_cellsizeUnit[1]
							, Mathr::symmetricRandom()*disorder_in_cellsizeUnit[2]
						    ) * 0.5 // *0.5 because symmetricRandom is (-1,1), and disorder is whole size where nodes can appear
					  )*cellsizeUnit_in_meters;

	if( 	   position[0] >= speciemen_size_in_meters[0] 
		|| position[1] >= speciemen_size_in_meters[1]
		|| position[2] >= speciemen_size_in_meters[2] )
		return false;

	Real radius 			= cellsizeUnit_in_meters*0.05;
	
	body->isDynamic			= true;
	
	physics->se3			= Se3r(position,q);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.8,0.8,0.8);
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= false;
	
	body->geometricalModel		= gSphere;
	body->physicalParameters	= physics;
	
	return true;
}


void LatticeExample::createBeam(shared_ptr<Body>& body, unsigned int i, unsigned int j)
{
	body = shared_ptr<Body>(new Body(0,beamGroupMask));
	shared_ptr<LatticeBeamParameters> physics(new LatticeBeamParameters);
	shared_ptr<LineSegment> gBeam(new LineSegment);
	
	Real length 			= 1.0; // unspecified for now, calcBeamsPositionOrientationLength will calculate it
	
	body->isDynamic			= true;
	
	physics->id1 			= i;
	physics->id2 			= j;

	gBeam->length			= length;
	gBeam->diffuseColor		= Vector3f(0.6,0.6,0.6);
	gBeam->wire			= false;
	gBeam->visible			= true;
	gBeam->shadowCaster		= false;
	
	body->geometricalModel		= gBeam;
	body->physicalParameters	= physics;
}


void LatticeExample::calcBeamPositionOrientationLength(shared_ptr<Body>& body)
{
	LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get());
	shared_ptr<Body>& bodyA = (*(rootBody->bodies))[beam->id1];
	shared_ptr<Body>& bodyB = (*(rootBody->bodies))[beam->id2];
	Se3r& se3A 		= bodyA->physicalParameters->se3;
	Se3r& se3B 		= bodyB->physicalParameters->se3;
	
	Se3r se3Beam;
	se3Beam.position 	= (se3A.position + se3B.position)*0.5;
	Vector3r dist 		= se3A.position - se3B.position;
	
	Real length 		= dist.normalize();
	beam->direction 	= dist;
	beam->initialDirection 	= dist;
	beam->length 		= length;
	beam->initialLength 	= length;
	
	beam->criticalTensileStrain 	= crit_TensileStrain_percent/100.0;
	beam->criticalCompressiveStrain = crit_ComprStrain_percent/100.0;
	beam->longitudalStiffness 	= longitudalStiffness_noUnit;
	beam->bendingStiffness 		= bendingStiffness_noUnit;
	
	se3Beam.orientation.align( Vector3r::UNIT_X , dist );
	beam->se3 		= se3Beam;
	beam->se3Displacement.position 	= Vector3r(0.0,0.0,0.0);
	beam->se3Displacement.orientation.align(dist,dist);
}

void LatticeExample::calcAxisAngle(LatticeBeamParameters* beam, BodyContainer* bodies, unsigned int otherId, InteractionContainer* ints, unsigned int thisId)
{ 
	if( ! ints->find(otherId,thisId) && otherId != thisId )
	{
		LatticeBeamParameters* 	otherBeam 		= dynamic_cast<LatticeBeamParameters*>( ((*(bodies))[ otherId ])->physicalParameters.get() );
		
	//	Quaternionr 		rotation;
	//	Vector3r 		axis;
		Real 			angle;
		
	//	rotation.align( beam->direction , otherBeam->direction );
	//	rotation.toAxisAngle (axis, angle);
	//	Vector3r result = axis*angle;
	
		angle = (beam->direction.cross(otherBeam->direction)[2] > 0.0 ? 1.0 : -1.0) * Mathr::aCos( beam->direction.dot(otherBeam->direction) / ( beam->direction.length() * otherBeam->direction.length() ) );

		shared_ptr<Interaction> 		interaction(new Interaction( thisId , otherId ));
		shared_ptr<LatticeBeamAngularSpring> 	angularSpring(new LatticeBeamAngularSpring);
		
	//	angularSpring->initialAngle 		= result;
	//	angularSpring->angle 			= result;
	//	angularSpring->initialAngle 		= rotation;
	//	angularSpring->angle 			= rotation;
		angularSpring->initialAngle 		= angle;
		angularSpring->angle 			= angle;
		
		interaction->isReal			= true;
		interaction->isNew 			= false;
		interaction->interactionPhysics 	= angularSpring;
		ints->insert(interaction);
	}
}

void LatticeExample::calcBeamAngles(Body* body, BodyContainer* bodies, InteractionContainer* ints)
{
	LatticeBeamParameters* beam 	= dynamic_cast<LatticeBeamParameters*>(body->physicalParameters.get());

	std::vector<unsigned int>::iterator i   = connections[beam->id1].begin();
	std::vector<unsigned int>::iterator end = connections[beam->id1].end();
	
	for( ; i != end ; ++i )
		calcAxisAngle(beam,bodies,*i,ints,body->getId());
	
	i   = connections[beam->id2].begin();
	end = connections[beam->id2].end();
	for( ; i != end ; ++i )
		calcAxisAngle(beam,bodies,*i,ints,body->getId());
}

void LatticeExample::createActors(shared_ptr<MetaBody>& )
{
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");

	shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher	= shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
	geometricalModelDispatcher->add("LatticeSetParameters","LatticeSetGeometry","LatticeSet2LatticeBeams");
	
	rootBody->engines.clear();
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<LatticeLaw>(new LatticeLaw));
	rootBody->engines.push_back(geometricalModelDispatcher);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
}	
 


void LatticeExample::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
	rootBody->isDynamic		= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<LatticeSetParameters> physics(new LatticeSetParameters);
	physics->se3			= Se3r(Vector3r(0,0,0),q);
	physics->beamGroupMask 		= beamGroupMask;
	physics->nodeGroupMask 		= nodeGroupMask;
	
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("LatticeSetGeometry"));
	gm->diffuseColor 		= Vector3f(1,1,1);
	gm->wire 			= false;
	gm->visible 			= true;
	gm->shadowCaster 		= true;
	
	rootBody->interactionGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 	= gm;
	rootBody->physicalParameters 	= physics;
}
	
 
void LatticeExample::imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real displacement)
{
	shared_ptr<DisplacementEngine> translationCondition = shared_ptr<DisplacementEngine>(new DisplacementEngine);
 	translationCondition->displacement  = displacement;
	direction.normalize();
 	translationCondition->translationAxis = direction;
	
	rootBody->engines.push_back((rootBody->engines)[rootBody->engines.size()-1]);
	(rootBody->engines)[rootBody->engines.size()-2]=(rootBody->engines)[rootBody->engines.size()-3];
	(rootBody->engines)[rootBody->engines.size()-3]=translationCondition;
	translationCondition->subscribedBodies.clear();
	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
	
		if( b->getGroupMask() & nodeGroupMask )
		{
			Vector3r pos = b->physicalParameters->se3.position;
			if(        pos[0] > min[0] 
				&& pos[1] > min[1] 
				&& pos[2] > min[2] 
				&& pos[0] < max[0] 
				&& pos[1] < max[1] 
				&& pos[2] < max[2] 
				&& (b->getGroupMask() & nodeGroupMask)
				)
			{
				b->isDynamic = false;
				b->geometricalModel->diffuseColor = Vector3f(0.3,0.3,0.3);
				translationCondition->subscribedBodies.push_back(b->getId());
			}
		}
	}
}

 
