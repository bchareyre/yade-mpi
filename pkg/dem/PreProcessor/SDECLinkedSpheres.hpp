/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/core/Body.hpp>

class SDECLinkedSpheres : public FileGenerator
{
	private	:
		Vector3r	 nbSpheres
				,gravity;

		Real		 minRadius
				,density
				,maxRadius
				,disorder
				,spacing
				,supportSize
				,dampingForce
				,dampingMomentum
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg;

		int		 timeStepUpdateInterval;

		bool		 momentRotationLaw
				,support1
				,support2;
	
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createSphere(shared_ptr<Body>& body, int i, int j, int k);
		void createActors(shared_ptr<World>& rootBody);
		void positionRootBody(shared_ptr<World>& rootBody);
	
	public :
		SDECLinkedSpheres ();
		~SDECLinkedSpheres ();
		bool generate();
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(FileGenerator,(nbSpheres)(minRadius)(maxRadius)(density)(sphereYoungModulus)(spherePoissonRatio)(sphereFrictionDeg)(dampingForce)(dampingMomentum)(momentRotationLaw)(gravity)(disorder)(spacing)(supportSize)(support1)(support2)(timeStepUpdateInterval));
	REGISTER_CLASS_NAME(SDECLinkedSpheres);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(SDECLinkedSpheres);


