/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_BOX_HPP
#define LATTICE_BOX_HPP 

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/pkg-common/BodyRedirectionVector.hpp>

class LatticeBeamParameters;
class StrainRecorder;
class MeasurePoisson;
class NodeRecorder;
class BeamRecorder;
class MovingSupport;

class LatticeExample : public FileGenerator
{
	private :
		int 		nodeGroupMask,beamGroupMask,quadGroupMask;
		
	// mesh generation	
		Vector3r 	 speciemen_size_in_meters 	// size
				,disorder_in_cellsizeUnit;	// s
		
                Real             maxLength_in_cellsizeUnit      // r
                                ,minAngle_betweenBeams_deg      // a
                                ,cellsizeUnit_in_meters         // g
                        // MaterialParameters of cement matrix  
                                ,crit_TensileStrain             // E_min
                                ,crit_ComprStrain               // E_max
                                
                                ,longitudalStiffness_noUnit     // k_l
                                ,bendingStiffness_noUnit        // k_b
                                ,torsionalStiffness_noUnit      // k_t
                        //////////////////////////////////////                          
                                ,nonLocalL_in_cellsizeUnit;     // l
                                
                bool             triangularBaseGrid
				,triangularBaseGrid3D
				,use_Delaunay
                                ,useNonLocalModel
                                ,useBendTensileSoftening
                                ,useStiffnessSoftening
                                ,ensure2D
                                ,roughEdges
				,calculate_Torsion
				,quads;
        
        // aggregates
                bool             useAggregates;
                Real             aggregatePercent
                                ,aggregateMeanDiameter
                                ,aggregateSigmaDiameter
                                ,aggregateMinDiameter
                                ,aggregateMaxDiameter
                        // MaterialParameters of aggregate
                                ,agg_longStiffness_noUnit       // k_l aggregate
                                ,agg_bendStiffness_noUnit       // k_b aggregate
                                ,agg_torsStiffness_noUnit       // k_t aggregate
                                ,agg_critCompressStrain         // E.c aggregate
                                ,agg_critTensileStrain          // E.l aggregate
                        // MaterialParameters of bond
                                ,bond_longStiffness_noUnit      // k_l bond
                                ,bond_bendStiffness_noUnit      // k_b bond
                                ,bond_torsStiffness_noUnit      // k_t bond
                                ,bond_critCompressStrain        // E.c bond
                                ,bond_critTensileStrain         // E.l bond
                        // MaterialParameters of steel fibres
                                ,fibre_longStiffness_noUnit      // k_l fibre
                                ,fibre_bendStiffness_noUnit      // k_b fibre
                                ,fibre_torsStiffness_noUnit      // k_t fibre
                                ,fibre_critCompressStrain        // E.c fibre
                                ,fibre_critTensileStrain         // E.l fibre
				,fibre_count
				,beams_per_fibre
				,fibre_allows
				,fibre_irregularity_noUnit
				,fibre_balancing_iterations
				
				,fibres_total
				,matrix_total
				,beam_total;

		std::vector<std::pair<Vector3r,Vector3r> > fibres; // start, direction

                void addAggregates(shared_ptr<MetaBody>& rootBody);
                void makeFibreBeams(shared_ptr<MetaBody>& rootBody);
		int isFibre(Vector3r,Vector3r);
		bool fibreAllows(Vector3r);

////////////////////////////////////////////////////
struct Circle
{
        float x,y,z,d;
};
		bool overlaps(Circle& cc,std::vector<Circle>& c);
		int aggInside(Vector3r& a,Vector3r& b,std::vector<Circle>& c, Real cellsizeUnit_in_meters);
		float aggsAreas(std::vector<Circle>& c);
		float aggsVolumes(std::vector<Circle>& c);
                
////////////////////////////////////////////
                
        // conditions
		Vector3r 	 region_A_min
			 	,region_A_max
			 	,direction_A
				 
			 	,region_B_min
			 	,region_B_max
			 	,direction_B
				 
			 	,region_C_min
			 	,region_C_max
			 	,direction_C
				 
			 	,region_D_min
			 	,region_D_max
			 	,direction_D
				 
			 	,region_E_min
			 	,region_E_max
			 	,direction_E
				 
			 	,region_F_min
			 	,region_F_max
			 	,direction_F;
				 
						
		Real		 displacement_A_meters
		 		,displacement_B_meters
                                ,displacement_C_meters
                                ,displacement_D_meters
                                ,displacement_E_meters
                                ,displacement_F_meters;
                                
        // strain recorder
                std::vector< unsigned int > subscribedBodies;
                Real             strainRecorder_xz_plane;
                Vector3r         strainRecorder_node1
                                ,strainRecorder_node2
                                ,measurePoisson_node3
                                ,measurePoisson_node4;
                shared_ptr<StrainRecorder> strainRecorder;
                shared_ptr<MeasurePoisson> measurePoisson;
		shared_ptr<BeamRecorder>   beamRecorder;
		shared_ptr<NodeRecorder>   nodeRecorder;
		shared_ptr<MovingSupport>  movingSupport;
                std::string      outputFile,poissonFile,nodeRecorderFile,beamRecorderFile;
                
        // delete beams regions
                Vector3r         regionDelete_A_min
                                ,regionDelete_A_max
                                ,regionDelete_B_min
                                ,regionDelete_B_max
                                ,regionDelete_C_min
                                ,regionDelete_C_max
                                ,regionDelete_D_min
                                ,regionDelete_D_max
                                ,regionDelete_E_min
                                ,regionDelete_E_max
                                ,regionDelete_F_min
                                ,regionDelete_F_max

                                ,regionDelete_1_min
                                ,regionDelete_1_max
                                ,regionDelete_2_min
                                ,regionDelete_2_max
                                ,regionDelete_3_min
                                ,regionDelete_3_max
                                ,regionDelete_4_min
                                ,regionDelete_4_max
                                ,regionDelete_5_min
                                ,regionDelete_5_max;

	// node recorder
		Vector3r	 nodeRec_A_min
				,nodeRec_A_max
				,nodeRec_B_min
				,nodeRec_B_max
				,nodeRec_C_min
				,nodeRec_C_max
				,nodeRec_D_min
				,nodeRec_D_max
				,nodeRec_E_min
				,nodeRec_E_max
				,nodeRec_F_min
				,nodeRec_F_max;
	// beam recorder
		
		Vector3r	 beamRec_A_pos
				,beamRec_B_pos
				,beamRec_C_pos
				,beamRec_D_pos
				,beamRec_E_pos;

		Real		 beamRec_A_range
				,beamRec_B_range
				,beamRec_C_range
				,beamRec_D_range
				,beamRec_E_range;

		int		 beamRec_A_dir
				,beamRec_B_dir
				,beamRec_C_dir
				,beamRec_D_dir
				,beamRec_E_dir;

	// moving support

		Vector3r	 movSupp_A_pos;
		Real		 movSupp_A_range;
		int		 movSupp_A_dir;

		Vector3r	 movSupp_B_pos;
		Real		 movSupp_B_range;
		int		 movSupp_B_dir;

		Vector3r	 movSupp_C_pos;
		Real		 movSupp_C_range;
		int		 movSupp_C_dir;

		Vector3r	 movSupp_D_pos;
		Real		 movSupp_D_range;
		int		 movSupp_D_dir;

        // non destroy areas
                Vector3r         nonDestroy_A_min
                                ,nonDestroy_A_max
                                ,nonDestroy_B_min
                                ,nonDestroy_B_max;

		Real		 nonDestroy_stiffness;

	// region of modified crit_TensileStrain
	
		Vector3r	 CT_A_min
				,CT_A_max
				,CT_B_min
				,CT_B_max;
		Real		 CT;

                                 
                std::vector< std::vector< unsigned int > > connections; // which node is in touch with what beams.
                                
        public : 
		LatticeExample();
		virtual ~LatticeExample();

		bool generate();
	
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		bool createNode(shared_ptr<Body>& body, int i, int j, int k);
		bool createNodeXYZ(shared_ptr<Body>& body, Real i, Real j, Real k);
		bool createQuad(shared_ptr<Body>& body, int i, int j, Vector3r);
                void createBeam(shared_ptr<Body>& body, int i, int j);
                Real calcBeamPositionOrientationLength(shared_ptr<Body>& body);
                bool notDeleted(Vector3r pos);
		bool isDeleted(Vector3r pos, Vector3r min, Vector3r max);
                void calcBeamAngles(Body* body, BodyContainer* bodies,InteractionContainer* ints);
                void calcAxisAngle(LatticeBeamParameters* beam, BodyContainer* bodies, int otherId,InteractionContainer* ints, int thisId);
                bool checkMinimumAngle(BodyRedirectionVector&,shared_ptr<Body>&);
                bool checkAngle( Vector3r , Vector3r& );
                void imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity);
                void regionDelete(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max);
                void nonDestroy(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max);
                void modifyCT(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max);
		void makeFibres();

                virtual void registerAttributes();
                REGISTER_CLASS_NAME(LatticeExample);
		REGISTER_BASE_CLASS_NAME(FileGenerator);

};

REGISTER_SERIALIZABLE(LatticeExample,false);

#endif // LATTICE_BOX_HPP 

