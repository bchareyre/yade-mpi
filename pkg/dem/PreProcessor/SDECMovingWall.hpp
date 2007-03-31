/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2005 by Andreas Plesch                                  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDEC_MOVING_WALL_HPP
#define SDEC_MOVING_WALL_HPP

#include<yade/core/FileGenerator.hpp>

class SDECMovingWall : public FileGenerator
{
	private :
		Vector3r	 nbSpheres
				,groundSize
				,groundPosition
				,wallSize
				,wallPosition
				,wallTranslationAxis
				,gravity
				,side1Size
				,side1Position
				,side2Size
				,side2Position
				,side3Size
				,side3Position;
		
		bool		 useSpheresAsGround
				,side1wire
				,side2wire
				,side3wire;

		bool		rotationBlocked;
		
		Real		 minRadius
				,density
				,maxRadius
				,wallVelocity
				,dampingForce
				,disorder
				,dampingMomentum
				,spheresHeight
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg;

		int timeStepUpdateInterval;
		
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents, bool wire);
		void createSphere(shared_ptr<Body>& body, int i, int j, int k);
		void createGroundSphere(shared_ptr<Body>& body,Real radius, Real i, Real j, Real k);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

	public :
		SDECMovingWall ();
		~SDECMovingWall ();
		string generate();

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();

	REGISTER_CLASS_NAME(SDECMovingWall);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(SDECMovingWall,false);

#endif // SDEC_MOVING_WALL_HPP 

