/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_BOX_HPP
#define LATTICE_BOX_HPP 

#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-package-common/BodyRedirectionVector.hpp>

class LatticeBeamParameters;
class StrainRecorder;
class MeasurePoisson;

class LatticeExample : public FileGenerator
{
	private :
		int 		nodeGroupMask,beamGroupMask;
		
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
                                ,useNonLocalModel
                                ,useBendTensileSoftening
                                ,useStiffnessSoftening
                                ,ensure2D
                                ,roughEdges;
        
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
                                ,bond_critTensileStrain;        // E.l bond

                void addAggregates(shared_ptr<MetaBody>& rootBody);
                
                
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
			 	,direction_D;
				 
						
		Real		 displacement_A_meters
		 		,displacement_B_meters
                                ,displacement_C_meters
                                ,displacement_D_meters;
                                
        // strain recorder
                std::vector< unsigned int > subscribedBodies;
                Real             strainRecorder_xz_plane;
                Vector3r         strainRecorder_node1
                                ,strainRecorder_node2
                                ,measurePoisson_node3
                                ,measurePoisson_node4;
                shared_ptr<StrainRecorder> strainRecorder;
                shared_ptr<MeasurePoisson> measurePoisson;
                std::string      outputFile,poissonFile;
                
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
                                ,regionDelete_F_max;

        // non destroy areas
                Vector3r         nonDestroy_A_min
                                ,nonDestroy_A_max
                                ,nonDestroy_B_min
                                ,nonDestroy_B_max;
                
                                 
                std::vector< std::vector< unsigned int > > connections; // which node is in touch with what beams.
                                
        public : 
		LatticeExample();
		virtual ~LatticeExample();

		string generate();
	
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		bool createNode(shared_ptr<Body>& body, int i, int j, int k);
                void createBeam(shared_ptr<Body>& body, unsigned int i, unsigned int j);
                void calcBeamPositionOrientationLength(shared_ptr<Body>& body);
                void calcBeamAngles(Body* body, BodyContainer* bodies,InteractionContainer* ints);
                void calcAxisAngle(LatticeBeamParameters* beam, BodyContainer* bodies, unsigned int otherId,InteractionContainer* ints,unsigned int thisId);
                bool checkMinimumAngle(BodyRedirectionVector&,shared_ptr<Body>&);
                bool checkAngle( Vector3r , Vector3r& );
                void imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity);
                void regionDelete(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max);
                void nonDestroy(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max);

                virtual void registerAttributes();
                REGISTER_CLASS_NAME(LatticeExample);
		REGISTER_BASE_CLASS_NAME(FileGenerator);

};

REGISTER_SERIALIZABLE(LatticeExample,false);

#endif // LATTICE_BOX_HPP 

