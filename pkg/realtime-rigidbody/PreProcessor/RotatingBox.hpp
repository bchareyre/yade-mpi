/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FileGenerator.hpp>

class RotatingBox : public FileGenerator
{
	private :
		Vector3r	 nbSpheres
				,nbBoxes
				,rotationAxis
				,gravity;

		Real		 minSize
				,maxSize
				,disorder
				,dampingForce
				,dampingMomentum
				,densityBox
				,densitySphere
				,rotationSpeed;

		bool		 isRotating
				,middleWireFrame;
	
		void createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool);
		void createSphere(shared_ptr<Body>& body, int i, int j, int k);
		void createBox(shared_ptr<Body>& body, int i, int j, int k);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
	
	public :
		RotatingBox ();
		~RotatingBox ();
		virtual bool generate();
	
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(RotatingBox);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(RotatingBox);


