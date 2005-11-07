/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDECTRIAXIALTEST_HPP
#define SDECTRIAXIALTEST_HPP

#include <yade/yade-core/FileGenerator.hpp>

class SDECTriaxialTest : public FileGenerator
{
	private	:
		Vector3r	 lowerCorner
				,upperCorner
				,nbSpheres;
	
		Real		 spacing
				,disorder
				,minRadius
				,maxRadius

				,boxYoungModulus
				,boxPoissonRatio
				,boxFrictionDeg

				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg
				,density
	
				,doesNothingYet_TM;

		void createSphere(shared_ptr<Body>& body, int i, int j, int k);
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

	public :
		SDECTriaxialTest();
		virtual ~SDECTriaxialTest();
		string generate();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(SDECTriaxialTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(SDECTriaxialTest,false);

#endif // SDECTRIAXIALTEST_HPP

