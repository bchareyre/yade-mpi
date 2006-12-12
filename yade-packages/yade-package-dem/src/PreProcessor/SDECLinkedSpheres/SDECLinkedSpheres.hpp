/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LINKEDSPHERES_HPP
#define LINKEDSPHERES_HPP

#include <yade/yade-core/FileGenerator.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <yade/yade-core/Body.hpp>

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
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public :
		SDECLinkedSpheres ();
		~SDECLinkedSpheres ();
		string generate();
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(SDECLinkedSpheres);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(SDECLinkedSpheres,false);

#endif // LINKEDSPHERES_HPP

