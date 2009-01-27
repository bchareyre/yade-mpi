/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FileGenerator.hpp>

#include <string>

class FEMBeam : public FileGenerator
{
	private :
		
		std::string	 femTxtFile;

		int		 nodeGroupMask
				,tetrahedronGroupMask;
		
		Vector3r	 gravity
				,regionMin1
				,regionMax1
				,translationAxis1
				,regionMin2
				,regionMax2
				,translationAxis2;

		Real		 velocity2
				,velocity1;

//	private : Real dampingForce;
//	private : Real dampingMomentum;

// construction
	public :
		FEMBeam ();
		~FEMBeam ();
		bool generate();

	private :
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		void imposeTranslation(shared_ptr<MetaBody>& rootBody, Vector3r min, Vector3r max, Vector3r direction, Real velocity);

	protected :
		void registerAttributes();

	REGISTER_CLASS_NAME(FEMBeam);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(FEMBeam);


