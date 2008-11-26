/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LINKEDSPHERES_HPP
#define LINKEDSPHERES_HPP

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/core/Body.hpp>

class ThreePointBending : public FileGenerator
{
	private	:
		std::string	 yadeFileWithSpheres;

		Real pistonVelocity
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
	
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public :
		ThreePointBending ();
		~ThreePointBending ();
		bool generate();
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(ThreePointBending);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(ThreePointBending);

#endif // LINKEDSPHERES_HPP

