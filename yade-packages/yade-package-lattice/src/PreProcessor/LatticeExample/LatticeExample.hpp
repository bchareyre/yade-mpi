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

class LatticeExample : public FileGenerator
{
	private :
		int 		nodeGroupMask,beamGroupMask;
		
	// mesh generation	
		Vector3r 	 speciemen_size_in_meters 	// size
				,disorder_in_cellsizeUnit;	// s
		
		Real 		 maxLength_in_cellsizeUnit	// r
				,minAngle_betweenBeams_deg 	// a
				,cellsizeUnit_in_meters 	// g
				
				,crit_TensileStrain_percent	// E_min
				,crit_ComprStrain_percent	// E_max
				
				,longitudalStiffness_noUnit 	// k_l
				,bendingStiffness_noUnit;	// k_b
				
		bool 		 triangularBaseGrid;
		
		
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

		virtual void registerAttributes();
		REGISTER_CLASS_NAME(LatticeExample);
		REGISTER_BASE_CLASS_NAME(FileGenerator);

};

REGISTER_SERIALIZABLE(LatticeExample,false);

#endif // LATTICE_BOX_HPP 

