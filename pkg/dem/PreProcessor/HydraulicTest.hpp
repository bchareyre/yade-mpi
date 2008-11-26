/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef HydraulicTest_HPP
#define HydraulicTest_HPP

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/core/Body.hpp>

class HydraulicTest : public FileGenerator
{
	private	:
		std::string	 yadeFileWithSpheres;
		Vector3r	 gravity;
		Real 		 radius;
		Real		 supportDepth
				,pistonWidth
				,pistonVelocity
				,dampingForce
				,dampingMomentum
				,linkKn
				,linkKs
				,linkMaxNormalForce
				,linkMaxShearForce
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg;

		int		 timeStepUpdateInterval;

		bool		 momentRotationLaw;
	
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		bool inside(Vector3r pos);
	
	public :
		HydraulicTest ();
		~HydraulicTest ();
		bool generate();
		string file;
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(HydraulicTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(HydraulicTest);

#endif // LINKEDSPHERES_HPP

