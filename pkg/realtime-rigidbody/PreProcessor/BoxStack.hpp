/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOXSTACK_HPP
#define BOXSTACK_HPP

#include<yade/core/FileGenerator.hpp>

class BoxStack : public FileGenerator
{
	private :
		Vector3r	 nbBoxes
				,boxSize
				,bulletPosition
				,bulletVelocity
				,gravity;

		Real		 boxDensity
				,bulletSize
				,bulletDensity
				,dampingForce
				,dampingMomentum;

		bool		kinematicBullet;
	
	public :
		BoxStack ();
		virtual ~BoxStack ();

		virtual void registerAttributes();
		
		virtual bool generate();

	private :
		void createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool);
		void createSphere(shared_ptr<Body>& body);
		void createBox(shared_ptr<Body>& body, int i, int j, int k);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

	REGISTER_CLASS_NAME(BoxStack);
	REGISTER_BASE_CLASS_NAME(FileGenerator);

};

REGISTER_SERIALIZABLE(BoxStack,false);

#endif // __BOXSTACK_H__


