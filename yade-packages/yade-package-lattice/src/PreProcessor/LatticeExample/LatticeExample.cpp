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
#include "NonLocalDependency.hpp"
#include "LatticeNodeParameters.hpp"
#include "LineSegment.hpp"
#include "LatticeLaw.hpp"
#include "StrainRecorder.hpp"
#include "MeasurePoisson.hpp"
#include "NonLocalInitializer.hpp"
#include <yade/yade-lib-base/yadeWm3Extra.hpp>


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
#include <yade/yade-package-common/Quadrilateral.hpp>
#include <yade/yade-package-common/ParticleParameters.hpp>

using namespace boost;
using namespace std;


LatticeExample::LatticeExample() : FileGenerator()
{
        nodeGroupMask           = 1;
        beamGroupMask           = 2;
	quadGroupMask		= 4;
        
        speciemen_size_in_meters = Vector3r(0.1,0.1,0.0001);
        cellsizeUnit_in_meters   = 0.003;
        minAngle_betweenBeams_deg= 20.0;
        disorder_in_cellsizeUnit = Vector3r(0.6,0.6,0.0);
        maxLength_in_cellsizeUnit= 1.9;
        triangularBaseGrid       = true;
        triangularBaseGrid3D     = true;
        useNonLocalModel         = false;
        useBendTensileSoftening  = false;
        useStiffnessSoftening    = false;
        nonLocalL_in_cellsizeUnit= 1.1;         // l
                                
        crit_TensileStrain       = 1.0; // E_min
        crit_ComprStrain         = 0.5; // E_max
                        
        longitudalStiffness_noUnit= 1.0;        // k_l
        bendingStiffness_noUnit   = 0.6;        // k_b
        torsionalStiffness_noUnit = 0.6;        // k_t
        
        ensure2D 		 = false;
        roughEdges 		 = false;
	calculate_Torsion	 = false;
	quads			 = false;
        
        region_A_min             = Vector3r(-0.006, 0.096,-1);
        region_A_max             = Vector3r( 0.16 , 0.16 , 1);
	direction_A 		 = Vector3r(0,1,0);
	displacement_A_meters	 = 0.0001;
	
	region_B_min 		 = Vector3r(-0.006,-0.006,-1);
	region_B_max 		 = Vector3r( 0.16 , 0.004, 1);
	direction_B 		 = Vector3r(0,-1,0);
	displacement_B_meters	 = 0.0001;

	region_C_min 		 = Vector3r(-0.006, 0.096,-1);
	region_C_max 		 = Vector3r( 0.16 , 0.16 , 1);
	direction_C 		 = Vector3r(0,1,0);
	displacement_C_meters	 = 0.0001;
	
	region_D_min 		 = Vector3r(-0.006,-0.006,-1);
	region_D_max 		 = Vector3r( 0.16 , 0.004, 1);
        direction_D              = Vector3r(0,-1,0);
        displacement_D_meters    = 0.0001;
        
        strainRecorder_xz_plane  = -1;
        strainRecorder_node1     = Vector3r(0,0,0);
        strainRecorder_node2     = Vector3r(0,1,0);
        measurePoisson_node3     = Vector3r(0  ,0.1,0);
        measurePoisson_node4     = Vector3r(0.2,0.1,0);
        outputFile               = "../data/strains";
        poissonFile              = "../data/poisson";
        subscribedBodies.clear();
                
        regionDelete_A_min       = Vector3r(0,0,0);
        regionDelete_A_max       = Vector3r(0,0,0);
        regionDelete_B_min       = Vector3r(0,0,0);
        regionDelete_B_max       = Vector3r(0,0,0);
        regionDelete_C_min       = Vector3r(0,0,0);
        regionDelete_C_max       = Vector3r(0,0,0);
        regionDelete_D_min       = Vector3r(0,0,0);
        regionDelete_D_max       = Vector3r(0,0,0);
        regionDelete_E_min       = Vector3r(0,0,0);
        regionDelete_E_max       = Vector3r(0,0,0);
        regionDelete_F_min       = Vector3r(0,0,0);
        regionDelete_F_max       = Vector3r(0,0,0);

        nonDestroy_A_min         = Vector3r(0,0,0);
        nonDestroy_A_max         = Vector3r(0,0,0);
        nonDestroy_B_min         = Vector3r(0,0,0);
        nonDestroy_B_max         = Vector3r(0,0,0);

        useAggregates            = false;
        aggregatePercent         = 40;
        aggregateMeanDiameter    = cellsizeUnit_in_meters*1;
        aggregateSigmaDiameter   = cellsizeUnit_in_meters*2;
        aggregateMinDiameter     = cellsizeUnit_in_meters*2;
        aggregateMaxDiameter     = cellsizeUnit_in_meters*4;
        // MaterialParameters of aggregate
        agg_longStiffness_noUnit = 3.0;                                 // k_l aggregate
        agg_bendStiffness_noUnit = 2.1;                                 // k_b aggregate
        agg_torsStiffness_noUnit = 2.1;                                 // k_b aggregate
        agg_critCompressStrain   = 100.0;                               // E.c aggregate
        agg_critTensileStrain    = 50.0;                                // E.l aggregate
        // MaterialParameters of bond
        bond_longStiffness_noUnit= 0.7;                                 // k_l bond
        bond_bendStiffness_noUnit= 0.28;                                // k_b bond
        bond_torsStiffness_noUnit= 0.28;                                // k_b bond
        bond_critCompressStrain  = 100.0;                               // E.c bond
        bond_critTensileStrain   = 50.0;                                // E.l bond
}


LatticeExample::~LatticeExample()
{

}


void LatticeExample::registerAttributes()
{
	REGISTER_ATTRIBUTE(speciemen_size_in_meters); 	// size
	REGISTER_ATTRIBUTE(cellsizeUnit_in_meters);	// g [m]  	- cell size
	REGISTER_ATTRIBUTE(minAngle_betweenBeams_deg); 	// a [deg] 	- min angle
        REGISTER_ATTRIBUTE(disorder_in_cellsizeUnit);   // s [-]        - disorder 
        REGISTER_ATTRIBUTE(maxLength_in_cellsizeUnit);  // r [-]        - max beam length
        
        REGISTER_ATTRIBUTE(crit_TensileStrain);         // E_min [%]    - default 0.02 %
        REGISTER_ATTRIBUTE(crit_ComprStrain);           // E_max [%]    - default 0.2 %
        REGISTER_ATTRIBUTE(longitudalStiffness_noUnit); // k_l [-]      - default 1.0
        REGISTER_ATTRIBUTE(bendingStiffness_noUnit);    // k_b [-]      - default 0.6
        REGISTER_ATTRIBUTE(torsionalStiffness_noUnit);  // k_t [-]      - default 0.6
        
        REGISTER_ATTRIBUTE(ensure2D);
        REGISTER_ATTRIBUTE(roughEdges);
        REGISTER_ATTRIBUTE(calculate_Torsion);
        REGISTER_ATTRIBUTE(quads);
        
        REGISTER_ATTRIBUTE(triangularBaseGrid);         //              - triangles
        REGISTER_ATTRIBUTE(triangularBaseGrid3D);       //              - triangles 3d
        REGISTER_ATTRIBUTE(useBendTensileSoftening);
        REGISTER_ATTRIBUTE(useStiffnessSoftening);
        REGISTER_ATTRIBUTE(useNonLocalModel);
        REGISTER_ATTRIBUTE(nonLocalL_in_cellsizeUnit);  // l
        
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
        
        REGISTER_ATTRIBUTE(strainRecorder_xz_plane);
        REGISTER_ATTRIBUTE(strainRecorder_node1);
        REGISTER_ATTRIBUTE(strainRecorder_node2);
        REGISTER_ATTRIBUTE(outputFile);
        REGISTER_ATTRIBUTE(measurePoisson_node3);
        REGISTER_ATTRIBUTE(measurePoisson_node4);
        REGISTER_ATTRIBUTE(poissonFile);
        
        REGISTER_ATTRIBUTE(regionDelete_A_min);
        REGISTER_ATTRIBUTE(regionDelete_A_max);
        REGISTER_ATTRIBUTE(regionDelete_B_min);
        REGISTER_ATTRIBUTE(regionDelete_B_max);
        REGISTER_ATTRIBUTE(regionDelete_C_min);
        REGISTER_ATTRIBUTE(regionDelete_C_max);
        REGISTER_ATTRIBUTE(regionDelete_D_min);
        REGISTER_ATTRIBUTE(regionDelete_D_max);
        REGISTER_ATTRIBUTE(regionDelete_E_min);
        REGISTER_ATTRIBUTE(regionDelete_E_max);
        REGISTER_ATTRIBUTE(regionDelete_F_min);
        REGISTER_ATTRIBUTE(regionDelete_F_max);

        REGISTER_ATTRIBUTE(nonDestroy_A_min);
        REGISTER_ATTRIBUTE(nonDestroy_A_max);
        REGISTER_ATTRIBUTE(nonDestroy_B_min);
        REGISTER_ATTRIBUTE(nonDestroy_B_max);

        REGISTER_ATTRIBUTE(useAggregates);
        REGISTER_ATTRIBUTE(aggregatePercent);
        REGISTER_ATTRIBUTE(aggregateMeanDiameter);
        REGISTER_ATTRIBUTE(aggregateSigmaDiameter);
        REGISTER_ATTRIBUTE(aggregateMinDiameter);
        REGISTER_ATTRIBUTE(aggregateMaxDiameter);
        // MaterialParameters of aggregate
        REGISTER_ATTRIBUTE(agg_longStiffness_noUnit);
        REGISTER_ATTRIBUTE(agg_bendStiffness_noUnit);
        REGISTER_ATTRIBUTE(agg_torsStiffness_noUnit);
        REGISTER_ATTRIBUTE(agg_critCompressStrain);
        REGISTER_ATTRIBUTE(agg_critTensileStrain);
        // MaterialParameters of bond
        REGISTER_ATTRIBUTE(bond_longStiffness_noUnit);
        REGISTER_ATTRIBUTE(bond_bendStiffness_noUnit);
        REGISTER_ATTRIBUTE(bond_torsStiffness_noUnit);
        REGISTER_ATTRIBUTE(bond_critCompressStrain);
        REGISTER_ATTRIBUTE(bond_critTensileStrain);
}

string LatticeExample::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->transientInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->physicalActions		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	
	shared_ptr<Body> body;
	
	Vector3r nbNodes = speciemen_size_in_meters / cellsizeUnit_in_meters;
	if(triangularBaseGrid)
		nbNodes[1] *= 1.15471; // bigger by sqrt(3)/2 factor
	if(triangularBaseGrid3D)
		nbNodes[2] *= 1.22475; // bigger by (1/3)*(sqrt(6)) factor

        unsigned int totalNodesCount = 0;

	{
		setMessage("creating nodes...");
		float all = nbNodes[0]*nbNodes[1]*nbNodes[2];
		float current = 0.0;

		for( int j=0 ; j<=nbNodes[1] ; j++ )
		{
			if(shouldTerminate()) return "";

			for( int i=0 ; i<=nbNodes[0] ; i++ )
				for( int k=0 ; k<=nbNodes[2] ; k++)
				{
					setProgress(current++/all);

					shared_ptr<Body> node;
					if(createNode(node,i,j,k) || quads)
						rootBody->bodies->insert(node), ++totalNodesCount;
				}
		}
	}

        BodyRedirectionVector bc;
        bc.clear();

	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator bi2;
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	int beam_counter = 0;
	float nodes_a=0;
	float nodes_all = rootBody->bodies->size();
	setMessage("creating beams...");
	double r  = maxLength_in_cellsizeUnit*cellsizeUnit_in_meters;
	double r2 = std::pow(r,2);
	for(  ; bi!=biEnd ; ++bi )  // loop over all nodes, to create beams
	{
		Body* bodyA = (*bi).get(); // first_node
	
		bi2 = bi;
		++bi2;
		nodes_a+=1.0;
		
		for( ; bi2!=biEnd ; ++bi2 )
		{
			if(shouldTerminate()) return "";

			Body* bodyB = (*bi2).get(); // all other nodes
			// warning - I'm assuming that there are ONLY Nodes in the rootBody
			LatticeNodeParameters* a = static_cast<LatticeNodeParameters*>(bodyA->physicalParameters.get());
			LatticeNodeParameters* b = static_cast<LatticeNodeParameters*>(bodyB->physicalParameters.get());
			
			//if ((a->se3.position - b->se3.position).SquaredLength() < std::pow(maxLength_in_cellsizeUnit*cellsizeUnit_in_meters,2) )  

			if (	   ( std::abs(a->se3.position[0] - b->se3.position[0])<= r )
				&& ( std::abs(a->se3.position[1] - b->se3.position[1])<= r )
				&& ( std::abs(a->se3.position[2] - b->se3.position[2])<= r )
				&& ((a->se3.position - b->se3.position).SquaredLength() < r2 ) )  
			{
				shared_ptr<Body> beam;
				createBeam(beam,bodyA->getId(),bodyB->getId());
				calcBeamPositionOrientationLength(beam);
				if(checkMinimumAngle(bc,beam))
				{
					if( ++beam_counter % 100 == 0 )
					{
						cerr << "creating beam: " << beam_counter << " , " << ((nodes_a/nodes_all)*100.0)  << " %\n"; 
						setProgress(std::pow(nodes_a/nodes_all,2));
					}
					
					bc.insert(beam);
				}
			}
                }
        }

        { // subscribe two nodes, that are monitored by strain recoder to get a measure of length
                bi    = rootBody->bodies->begin();
                biEnd = rootBody->bodies->end();
                int node1Id=-1; Real len1=100000; Real tmpLen;
                int node2Id=-1; Real len2=100000;
                for(  ; bi!=biEnd ; ++bi )  // loop over all nodes, to find those closest to strainRecorder_node1 and strainRecorder_node2
                {
			if(shouldTerminate()) return "";

                        Body* body = (*bi).get();
                        LatticeNodeParameters* node = dynamic_cast<LatticeNodeParameters*>(body->physicalParameters.get());
                        tmpLen = ( strainRecorder_node1 - node->se3.position ).SquaredLength();
                        if(tmpLen < len1) len1=tmpLen, node1Id=body->getId();
                        tmpLen = ( strainRecorder_node2 - node->se3.position ).SquaredLength();
                        if(tmpLen < len2) len2=tmpLen, node2Id=body->getId();
                }
                assert(node1Id!=-1 && node2Id!=-1);
                subscribedBodies.push_back(node1Id); // bottom
                subscribedBodies.push_back(node2Id); // upper
                
                measurePoisson->bottom =node1Id; // bottom
                measurePoisson->upper  =node2Id; // upper
        }
        
        { // subscribe two nodes, that are monitored by MeasurePoisson to get a measure of poisson
                bi    = rootBody->bodies->begin();
                biEnd = rootBody->bodies->end();
                int node3Id=-1; Real len1=100000; Real tmpLen;
                int node4Id=-1; Real len2=100000;
                for(  ; bi!=biEnd ; ++bi )  // loop over all nodes, to find those closest to measurePoisson_node3 and measurePoisson_node4
                {
			if(shouldTerminate()) return "";

                        Body* body = (*bi).get();
                        LatticeNodeParameters* node = dynamic_cast<LatticeNodeParameters*>(body->physicalParameters.get());
                        tmpLen = ( measurePoisson_node3 - node->se3.position ).SquaredLength();
                        if(tmpLen < len1) len1=tmpLen, node3Id=body->getId();
                        tmpLen = ( measurePoisson_node4 - node->se3.position ).SquaredLength();
                        if(tmpLen < len2) len2=tmpLen, node4Id=body->getId();
                }
                assert(node3Id!=-1 && node4Id!=-1);
                measurePoisson->left   =node3Id; // left
                measurePoisson->right  =node4Id; // right
                measurePoisson->horizontal              =   (*(rootBody->bodies))[node4Id]->physicalParameters->se3.position[0] 
                                                          - (*(rootBody->bodies))[node3Id]->physicalParameters->se3.position[0];
        }


        bi    = bc.begin();
        biEnd = bc.end();
        for(  ; bi!=biEnd ; ++bi )  // loop over all newly created beams ...
        {
		if(shouldTerminate()) return "";

                shared_ptr<Body> b = *bi;
                rootBody->bodies->insert(b); // .. to insert them into rootBody
                
                // attach them to strain recorder
                
                Real xz_plane = strainRecorder_xz_plane;
                Real pos1 = (*(rootBody->bodies))[static_cast<LatticeBeamParameters*>( b->physicalParameters.get() )->id1]->physicalParameters->se3.position[1]; // beam first node
                Real pos2 = (*(rootBody->bodies))[static_cast<LatticeBeamParameters*>( b->physicalParameters.get() )->id2]->physicalParameters->se3.position[1]; // beam second node
                if( pos1 < pos2 )
                        std::swap(pos1,pos2); // make sure that pos1 is bigger 
                if(        pos1 > xz_plane
                        && pos2 < xz_plane 
                        )
                        subscribedBodies.push_back( b->getId() ); // beam crosses the plane!
        }
        strainRecorder->subscribedBodies        = subscribedBodies;

        strainRecorder->initialLength           =   (*(rootBody->bodies))[subscribedBodies[0]]->physicalParameters->se3.position[1] 
                                                  - (*(rootBody->bodies))[subscribedBodies[1]]->physicalParameters->se3.position[1];
        
        measurePoisson->vertical                =   (*(rootBody->bodies))[subscribedBodies[1]]->physicalParameters->se3.position[1] 
                                                  - (*(rootBody->bodies))[subscribedBodies[0]]->physicalParameters->se3.position[1];
                
        { // remember what node is in contact with what beams
                //    node                   beams
		connections.resize(totalNodesCount);
		
		bi    = rootBody->bodies->begin();
		biEnd = rootBody->bodies->end();
		
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			if(shouldTerminate()) return "";

			Body* body = (*bi).get();
			if( ! ( body->getGroupMask() & beamGroupMask ) )
				continue; // skip non-beams
			
			LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );
			connections[beam->id1].push_back(body->getId());
			connections[beam->id2].push_back(body->getId());
		}
	}

	setMessage("angular springs...");
	{ // create angular springs between beams
		bi    = rootBody->bodies->begin();
		biEnd = rootBody->bodies->end();
		float all_bodies = rootBody->bodies->size();
		int current = 0;
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			if(shouldTerminate()) return "";

			if( ++current % 100 == 0 )
			{
				cerr << "angular springs: " << current << " , " << ((static_cast<float>(current)/all_bodies)*100.0) << " %\n";
				setProgress(((float)(current)/all_bodies));
			}
				
			Body* body = (*bi).get();
			if( ! ( body->getGroupMask() & beamGroupMask ) )
				continue; // skip non-beams
				
			calcBeamAngles(body,rootBody->bodies.get(),rootBody->persistentInteractions.get());
                }
        }
	
	if(quads)
	{
		float all = (nbNodes[0]-1)*(nbNodes[1]-1)*(nbNodes[2]-1);
		float current = 0.0;
		setMessage("quadrilaterals...");
		for( int j=1 ; j<=nbNodes[1] ; j++ )
		{
			if(shouldTerminate()) return "";

			for( int i=1 ; i<=nbNodes[0] ; i++ )
			{
				shared_ptr<Body> quad;
				if(createQuad(quad,i,j,nbNodes))
					rootBody->bodies->insert(quad);
				setProgress(current++/all);
			}
		}
	};
        
        
        regionDelete(rootBody,regionDelete_A_min,regionDelete_A_max);
        regionDelete(rootBody,regionDelete_B_min,regionDelete_B_max);
        regionDelete(rootBody,regionDelete_C_min,regionDelete_C_max);
        regionDelete(rootBody,regionDelete_D_min,regionDelete_D_max);
        regionDelete(rootBody,regionDelete_E_min,regionDelete_E_max);
        regionDelete(rootBody,regionDelete_F_min,regionDelete_F_max);

        imposeTranslation(rootBody,region_A_min,region_A_max,direction_A,displacement_A_meters);
        imposeTranslation(rootBody,region_B_min,region_B_max,direction_B,displacement_B_meters);
        imposeTranslation(rootBody,region_C_min,region_C_max,direction_C,displacement_C_meters);
        imposeTranslation(rootBody,region_D_min,region_D_max,direction_D,displacement_D_meters);

        if(useAggregates) addAggregates(rootBody);
        
        nonDestroy(rootBody,nonDestroy_A_min,nonDestroy_A_max);
        nonDestroy(rootBody,nonDestroy_B_min,nonDestroy_B_max);
        
        cerr << "finished.. saving\n";

        return "Number of nodes created:\n" + lexical_cast<string>(nbNodes[0]) + ","
                                            + lexical_cast<string>(nbNodes[1]) + ","
                                            + lexical_cast<string>(nbNodes[2]) + "\n\nNOTE: sometimes it can look better when 'drawWireFrame' is enabled in Display tab.";

}

/// returns true if angle is bigger than minAngle_betweenBeams_deg
bool LatticeExample::checkAngle( Vector3r a, Vector3r& b)
{
	Quaternionr al;
	al.Align(a,b);
	Vector3r axis;
	Real angle;
	al.ToAxisAngle(axis, angle);
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
	q.FromAxisAngle( Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()) , Mathr::UnitRandom()*Mathr::PI );
	
	float  triang_x = triangularBaseGrid   ? ((static_cast<float>(j%2))*0.5)+i : i+0;
	double triang_y = triangularBaseGrid   ? 0.86602540378443864676*j          : j*1; // sqrt(3)/2
	double triang_z = k;

	if(triangularBaseGrid3D)
	{
		triang_z *= 0.81649658092772603273;
		switch(k%3)
		{
			case 0 : triang_x += 0.5; triang_y +=     0.86602540378443864676/3.0; break;
			case 1 :                  triang_y += 2.0*0.86602540378443864676/3.0; break;
			case 2 : break;
		}
	}
	
	Vector3r position		= ( Vector3r(triang_x,triang_y,triang_z)
					  + Vector3r( 	  Mathr::SymmetricRandom()*disorder_in_cellsizeUnit[0]
					  		, Mathr::SymmetricRandom()*disorder_in_cellsizeUnit[1]
							, Mathr::SymmetricRandom()*disorder_in_cellsizeUnit[2]
						    ) * 0.5 // *0.5 because symmetricRandom is (-1,1), and disorder is whole size where nodes can appear
					  )*cellsizeUnit_in_meters;

	Real radius 			= cellsizeUnit_in_meters*0.05;
	
	body->isDynamic			= true;
	
	physics->se3			= Se3r(position,q);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.8,0.8,0.8);
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= false;
	
        body->geometricalModel          = gSphere;
        body->physicalParameters        = physics;

//////////////////////
// some custom coordinates for testing: - FIXME - it will be useful to be able to get coordinates from outside
//static int zzzzz=0;
//switch(zzzzz%5)
//{
//      case 0  : physics->se3.position=Vector3r(0,0.4,1.5); break;
//      case 1  : physics->se3.position=Vector3r(0,0.8,0.6); break;
//      case 2  : physics->se3.position=Vector3r(0,0  ,0  ); break;
//      case 3  : physics->se3.position=Vector3r(0,1.6,0.5); break;
//      case 4  : physics->se3.position=Vector3r(0,2.0,0  ); break;
//};
//zzzzz++;
//
//////////////////////
        
	if( 	   position[0] >= speciemen_size_in_meters[0] 
		|| position[1] >= speciemen_size_in_meters[1]
		|| position[2] >= speciemen_size_in_meters[2] )
		return false;

        return true;
}


bool LatticeExample::createQuad(shared_ptr<Body>& quad, int i, int j, Vector3r nbNodes)
{
	quad = shared_ptr<Body>(new Body(0,quadGroupMask));
	shared_ptr<Quadrilateral> gQuad(new Quadrilateral( (j-1)*(int)std::floor(nbNodes[0]+1)+i-1 , j*(int)std::floor(nbNodes[0]+1)+i-1 , j*(int)std::floor(nbNodes[0]+1)+i , (j-1)*(int)std::floor(nbNodes[0]+1)+i, rootBody.get()));
	// FIXME FIXME - this is an empty class. not needed at all.
	// all this Quadrilateral stuff in fact does not fit current design at all.
	shared_ptr<ParticleParameters> physics(new ParticleParameters);
	
	quad->isDynamic			= false;

	physics->se3			= Se3r(Vector3r(0,0,0),Quaternionr(1,0,0,0));
	physics->mass			= 0;
	physics->velocity		= Vector3r(0,0,0);

	gQuad->diffuseColor		= Vector3f(0.0,0.0,0.0);
	gQuad->visible			= true;
	gQuad->wire			= false;
	gQuad->shadowCaster		= false;
	
        quad->geometricalModel          = gQuad;
	quad->physicalParameters	= physics;

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
	
	Real length 		= dist.Normalize();
	beam->direction 	= dist;
        beam->length            = length;
        beam->initialLength     = length;
        
        beam->criticalTensileStrain     = crit_TensileStrain;
        beam->criticalCompressiveStrain = crit_ComprStrain;
        beam->longitudalStiffness       = longitudalStiffness_noUnit;
        beam->bendingStiffness          = bendingStiffness_noUnit;
        beam->torsionalStiffness        = torsionalStiffness_noUnit;
        
	se3Beam.orientation.Align( Vector3r::UNIT_X , dist );
	beam->se3 		= se3Beam;
	beam->se3Displacement.position 	= Vector3r(0.0,0.0,0.0);
	beam->se3Displacement.orientation.Align(dist,dist);

	beam->otherDirection	= beam->se3.orientation*Vector3r::UNIT_Y; // any unit vector that is orthogonal to direction.
}

void LatticeExample::calcAxisAngle(LatticeBeamParameters* beam1, BodyContainer* bodies, unsigned int otherId, InteractionContainer* ints, unsigned int thisId)
{ 
	if( ! ints->find(otherId,thisId) && otherId != thisId )
	{
		LatticeBeamParameters* 	beam2 		= static_cast<LatticeBeamParameters*>( ((*(bodies))[ otherId ])->physicalParameters.get() );
		Real 			angle, offPlaneAngle;
		
		angle = unitVectorsAngle(beam1->direction,beam2->direction);

                shared_ptr<Interaction>                 interaction(new Interaction( thisId , otherId ));
                shared_ptr<LatticeBeamAngularSpring>    angularSpring(new LatticeBeamAngularSpring);
		
		angularSpring->initialPlaneAngle 	= angle;
		angularSpring->planeSwap180		= false;
		angularSpring->lastCrossProduct		= 1.0*(beam1->direction.Cross(beam2->direction));
		angularSpring->lastCrossProduct.Normalize();
		
		angularSpring->initialOffPlaneAngle1	= unitVectorsAngle(beam1->otherDirection,angularSpring->lastCrossProduct);
		angularSpring->initialOffPlaneAngle2	= unitVectorsAngle(beam2->otherDirection,angularSpring->lastCrossProduct);

	//	std::cerr << thisId << ", " << otherId << ", (" << beam1->otherDirection << ") (" << beam2->otherDirection << ") (" << angularSpring->lastCrossProduct << ")\n";
				
		Quaternionr	aligner1,aligner2;
		aligner1.FromAxisAngle(beam1->direction , angularSpring->initialOffPlaneAngle1);
		aligner2.FromAxisAngle(beam2->direction , angularSpring->initialOffPlaneAngle2);
				
		Vector3r	dir1			= aligner1 * angularSpring->lastCrossProduct;
		Vector3r	dir2			= aligner2 * angularSpring->lastCrossProduct;

		// FIXME
		if( dir1.Dot(beam1->otherDirection) < 0.999999 )
			angularSpring->initialOffPlaneAngle1   *= -1.0;//, angularSpring->offPlaneSwap1 = true;
		if( dir2.Dot(beam2->otherDirection) < 0.999999 )
			angularSpring->initialOffPlaneAngle2   *= -1.0;//, angularSpring->offPlaneSwap2 = true;
	
		interaction->isReal			= true;
		interaction->isNew 			= false;
		interaction->interactionPhysics 	= angularSpring;
		ints->insert(interaction);
	}
}

void LatticeExample::calcBeamAngles(Body* body, BodyContainer* bodies, InteractionContainer* ints)
{
	LatticeBeamParameters* beam 	= static_cast<LatticeBeamParameters*>(body->physicalParameters.get());

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
        shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher   = shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");

        shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher       = shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
        geometricalModelDispatcher->add("LatticeSetParameters","LatticeSetGeometry","LatticeSet2LatticeBeams");
        
        strainRecorder = shared_ptr<StrainRecorder>(new StrainRecorder);
        strainRecorder->outputFile              = outputFile;
        strainRecorder->interval                = 10;
        
        measurePoisson = shared_ptr<MeasurePoisson>(new MeasurePoisson);
        measurePoisson->outputFile              = poissonFile;
        measurePoisson->interval                = 10;
        
        shared_ptr<LatticeLaw> latticeLaw(new LatticeLaw);
        latticeLaw->ensure2D   = ensure2D;
        latticeLaw->roughEdges = roughEdges;
        latticeLaw->calcTorsion= calculate_Torsion;
        
        rootBody->engines.clear();
        rootBody->engines.push_back(boundingVolumeDispatcher);
        rootBody->engines.push_back(latticeLaw);
        rootBody->engines.push_back(geometricalModelDispatcher);
        rootBody->engines.push_back(strainRecorder);
        rootBody->engines.push_back(measurePoisson);
        
        rootBody->initializers.clear();
        rootBody->initializers.push_back(boundingVolumeDispatcher);
        rootBody->initializers.push_back(geometricalModelDispatcher);

        if(useNonLocalModel)
        {
                shared_ptr<NonLocalInitializer> nonLocalInitializer(new NonLocalInitializer);
                nonLocalInitializer->range = nonLocalL_in_cellsizeUnit * cellsizeUnit_in_meters;
                rootBody->initializers.push_back(nonLocalInitializer);
        }
}       

void LatticeExample::positionRootBody(shared_ptr<MetaBody>& rootBody)
{
        rootBody->isDynamic             = false;

	Quaternionr q;
	q.FromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<LatticeSetParameters> physics(new LatticeSetParameters);
        physics->se3                    = Se3r(Vector3r(0,0,0),q);
        physics->beamGroupMask          = beamGroupMask;
        physics->nodeGroupMask          = nodeGroupMask;
        physics->useBendTensileSoftening= useBendTensileSoftening;
        physics->useStiffnessSoftening  = useStiffnessSoftening;
        
        shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
        
	set->diffuseColor		= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor		= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm = dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("LatticeSetGeometry"));
	gm->diffuseColor 		= Vector3f(1,1,1);
	gm->wire 			= false;
	gm->visible 			= true;
	gm->shadowCaster 		= true;
	
	rootBody->interactingGeometry	= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume	= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 	= gm;
	rootBody->physicalParameters 	= physics;
}
	
 
void LatticeExample::imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real displacement)
{
	shared_ptr<DisplacementEngine> translationCondition = shared_ptr<DisplacementEngine>(new DisplacementEngine);
 	translationCondition->displacement  = displacement;
	direction.Normalize();
 	translationCondition->translationAxis = direction;
        
        rootBody->engines.push_back((rootBody->engines)[rootBody->engines.size()-1]);
        (rootBody->engines)[rootBody->engines.size()-2]=(rootBody->engines)[rootBody->engines.size()-3];
        (rootBody->engines)[rootBody->engines.size()-3]=(rootBody->engines)[rootBody->engines.size()-4];
        (rootBody->engines)[rootBody->engines.size()-4]=translationCondition;
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

void LatticeExample::regionDelete(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max)
{
        vector<unsigned int> futureDeletes;
        
        BodyContainer::iterator bi    = rootBody->bodies->begin();
        BodyContainer::iterator biEnd = rootBody->bodies->end();
        for(  ; bi!=biEnd ; ++bi )
        {
                shared_ptr<Body> b = *bi;
        
                if( b->getGroupMask() & beamGroupMask )
                {
                        Vector3r pos = b->physicalParameters->se3.position;
                        if(        pos[0] > min[0] 
                                && pos[1] > min[1] 
                                && pos[2] > min[2] 
                                && pos[0] < max[0] 
                                && pos[1] < max[1] 
                                && pos[2] < max[2] 
                                )
                                futureDeletes.push_back( b->getId() );
                }
        }
        
        vector<unsigned int>::iterator vend = futureDeletes.end();
        for( vector<unsigned int>::iterator vsta = futureDeletes.begin() ; vsta != vend ; ++vsta)
                rootBody->bodies->erase(*vsta); 
}

void LatticeExample::nonDestroy(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max)
{
        vector<unsigned int> marked;
        
        BodyContainer::iterator bi    = rootBody->bodies->begin();
        BodyContainer::iterator biEnd = rootBody->bodies->end();
        for(  ; bi!=biEnd ; ++bi )
        {
                shared_ptr<Body> b = *bi;
        
                if( b->getGroupMask() & beamGroupMask )
                {
                        Vector3r pos = b->physicalParameters->se3.position;
                        if(        pos[0] > min[0] 
                                && pos[1] > min[1] 
                                && pos[2] > min[2] 
                                && pos[0] < max[0] 
                                && pos[1] < max[1] 
                                && pos[2] < max[2] 
                                )
                                marked.push_back( b->getId() );
                }
        }
        
        vector<unsigned int>::iterator vend = marked.end();
        for( vector<unsigned int>::iterator vsta = marked.begin() ; vsta != vend ; ++vsta)
        {
                LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>( ((*(rootBody->bodies))[*vsta])->physicalParameters.get());
                beam->criticalTensileStrain     = 0.9;
                beam->criticalCompressiveStrain = 0.9;
                beam->longitudalStiffness       = 4.0;
                beam->bendingStiffness          = 2.8;
                beam->torsionalStiffness        = 2.8;
                (*(rootBody->bodies))[beam->id1]->geometricalModel->diffuseColor = Vector3f(0.2,0.5,0.7);
                (*(rootBody->bodies))[beam->id2]->geometricalModel->diffuseColor = Vector3f(0.2,0.5,0.7);
        }
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

struct Circle
{
        float x,y,d;
};

bool overlaps(Circle& cc,std::vector<Circle>& c)
{
        std::vector<Circle>::iterator end=c.end();
        for(std::vector<Circle>::iterator i=c.begin();i!=end;++i)
        {
                float dist2 = std::pow(i->x - cc.x ,2)+std::pow(i->y - cc.y,2);
                float r2    = std::pow( 1.1*(i->d+cc.d)/2.0 ,2); // FIXME - 1.1 is hardcoded. van Mier's min distance is 1.1*(D1+D2)/2
                if(dist2<r2)
                        return true;
        }
        return false;
};

int aggInside(Vector3r& a,Vector3r& b,std::vector<Circle>& c, Real cellsizeUnit_in_meters)
{ // checks if nodes 'a','b' are inside any of aggregates from list 'c'
        int res=0;
        std::vector<Circle>::iterator end=c.end();
        for(std::vector<Circle>::iterator i=c.begin();i!=end;++i)
        {
        //      if(i->r < cellsizeUnit_in_meters) // FIXME
        //              continue;

                float dist2 = std::pow(i->x - a[0],2)+std::pow(i->y - a[1],2);
                float r2    = std::pow(i->d*0.5,2);
                if(dist2<r2) res=1; else res=0;

                dist2 = std::pow(i->x - b[0],2)+std::pow(i->y - b[1],2);
                if(dist2<r2) ++res;

                if(res!=0) return res;
        }
        return false;
}

float aggsAreas(std::vector<Circle>& c)
{
        float aggArea=0.0;
        std::vector<Circle>::iterator end=c.end();
        for(std::vector<Circle>::iterator i=c.begin();i!=end;++i)
                aggArea += 3.14159265358979323846*std::pow(i->d*0.5 ,2);
        return aggArea;
}

// random
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>

void LatticeExample::addAggregates(shared_ptr<MetaBody>& rootBody)
{
        // first make a list of circles
        std::vector<Circle> c;

        Real AGGREGATES_X=speciemen_size_in_meters[0];
        Real AGGREGATES_Y=speciemen_size_in_meters[1];
        Real MAX_DIAMETER  =aggregateMaxDiameter;
        Real MIN_DIAMETER  =aggregateMinDiameter;
        Real MEAN_DIAMETER =aggregateMeanDiameter;
        Real SIGMA_DIAMETER=aggregateSigmaDiameter;

        typedef boost::minstd_rand StdGenerator;
        static StdGenerator generator;
        static boost::variate_generator<StdGenerator&, boost::uniform_real<> >
                random1(generator, boost::uniform_real<>(0,1));
        static boost::variate_generator<StdGenerator&, boost::normal_distribution<> > 
                randomN(generator, boost::normal_distribution<>(MEAN_DIAMETER,SIGMA_DIAMETER));

        std::cerr << "generating aggregates ... ";
	setMessage(  "generating aggregates...");
        do
        {
                Circle cc;
                cc.x=random1()*AGGREGATES_X, cc.y=random1()*AGGREGATES_Y;
                do { cc.d=randomN(); } while (cc.d>=MAX_DIAMETER || cc.d<=MIN_DIAMETER);
                for(int i=0 ; i<1000 ; ++i)
                        if(overlaps(cc,c))
                                cc.x=random1()*AGGREGATES_X, cc.y=random1()*AGGREGATES_Y;
                        else
                        {
                                c.push_back(cc);
                //              std::cerr << cc.x << " " << cc.y << " " << cc.d << "\n";
                                break;
                        }
		setProgress((aggsAreas(c)/(AGGREGATES_X*AGGREGATES_Y))/(aggregatePercent/100.0));
        }
        while(aggregatePercent/100.0 > aggsAreas(c)/(AGGREGATES_X*AGGREGATES_Y) );

        std::cerr << "done. " << c.size() << " aggregates generated, area: " << aggsAreas(c)/(AGGREGATES_X*AGGREGATES_Y) << "\n";

        { // set different properties for beams that lie in an aggregate
          // parametrize from above - takes three arguments: 
          // - MaterialParameters of aggregate, 
          // - MaterialParameters of bond, 
          // - list of circles.
                BodyContainer::iterator bi    = rootBody->bodies->begin();
                BodyContainer::iterator biEnd = rootBody->bodies->end();
                float all_bodies = rootBody->bodies->size();
                int current = 0;
                for(  ; bi!=biEnd ; ++bi )  // loop over all beams
                {
                //      if( ++current % 100 == 0 )
                //              cerr << "aggregate beams: " << current << " , " << ((static_cast<float>(current)/all_bodies)*100.0) << " %\n";
                                
                        Body* body = (*bi).get();
                        if( ! ( body->getGroupMask() & beamGroupMask ) )
                                continue; // skip non-beams

                        LatticeBeamParameters* beam     = static_cast<LatticeBeamParameters*>(body->physicalParameters.get());
                        int ovv = aggInside(     (*(rootBody->bodies))[beam->id1]->physicalParameters->se3.position
                                                ,(*(rootBody->bodies))[beam->id2]->physicalParameters->se3.position
                                                ,c, cellsizeUnit_in_meters );
                        if(ovv==2) // aggregate
                        {
                                beam->longitudalStiffness       = agg_longStiffness_noUnit;
                                beam->bendingStiffness          = agg_bendStiffness_noUnit;
                                beam->torsionalStiffness        = agg_torsStiffness_noUnit;
                                beam->criticalTensileStrain     = agg_critTensileStrain;
                                beam->criticalCompressiveStrain = agg_critCompressStrain;
                
                                (*(rootBody->bodies))[beam->id1]->geometricalModel->diffuseColor = Vector3f(0.6,0.2,0.0);
                                (*(rootBody->bodies))[beam->id2]->geometricalModel->diffuseColor = Vector3f(0.6,0.2,0.0);
                        }
                        else if(ovv==1) // bond
                        {
                                beam->longitudalStiffness       = bond_longStiffness_noUnit;
                                beam->bendingStiffness          = bond_bendStiffness_noUnit;
                                beam->torsionalStiffness	= bond_torsStiffness_noUnit;
                                beam->criticalTensileStrain     = bond_critTensileStrain;
                                beam->criticalCompressiveStrain = bond_critCompressStrain;
                
                                (*(rootBody->bodies))[beam->id1]->geometricalModel->diffuseColor = Vector3f(0.6,0.6,0.0);
                                (*(rootBody->bodies))[beam->id2]->geometricalModel->diffuseColor = Vector3f(0.6,0.6,0.0);
                        }
                }
        }
}

