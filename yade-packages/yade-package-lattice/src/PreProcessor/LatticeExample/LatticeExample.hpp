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

class LatticeExample : public FileGenerator
{
	private :
		int 		nodeGroupMask,beamGroupMask;
		
		Vector3r 	 nbNodes
			 	,regionA_min
			 	,regionA_max
			 	,direction_A
			 	,regionB_min
			 	,regionB_max
			 	,direction_B;
		
		Real 		 disorder
		 		,maxLength
		 		,velocity_B
		 		,velocity_A
		 		,maxDeformationSquared;

	public : 
		LatticeExample();
		virtual ~LatticeExample();

		string generate();
	
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		void createNode(shared_ptr<Body>& body, int i, int j, int k);
		void createBeam(shared_ptr<Body>& body, unsigned int i, unsigned int j);
		void calcBeamsPositionOrientationLength(shared_ptr<MetaBody>& body);
		void imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity);

		virtual void registerAttributes();
		REGISTER_CLASS_NAME(LatticeExample);
		REGISTER_BASE_CLASS_NAME(FileGenerator);

};

REGISTER_SERIALIZABLE(LatticeExample,false);

#endif // LATTICE_BOX_HPP 

