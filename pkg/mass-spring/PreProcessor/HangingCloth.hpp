/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef HANGINGCLOTH_HPP
#define HANGINGCLOTH_HPP

#include<yade/core/FileGenerator.hpp>

class HangingCloth : public FileGenerator
{
	private : 
		shared_ptr<Interaction>  spring;

		int		 width
				,height
				,cellSize;

		Real		 springStiffness
				,springDamping
				,particleDamping
				,clothMass
				,dampingForce
				,sphereYoungModulus
				,spherePoissonRatio
				,sphereFrictionDeg;

		bool		 fixPoint1
				,fixPoint2
				,fixPoint3
				,fixPoint4
				,ground;
// spheres
		Vector3r	 nbSpheres
				,gravity;

		Real		 density
				,minRadius
				,maxRadius
				,disorder
				,spacing
				,dampingMomentum;

		bool		linkSpheres;

	public :
		HangingCloth ();
		~HangingCloth ();
		bool generate();

	
	private :
		shared_ptr<Interaction>& createSpring(const shared_ptr<MetaBody>& rootBody,int i,int j);
		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();

	REGISTER_CLASS_NAME(HangingCloth);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(HangingCloth);

#endif // __HANGINGCLOTH_H__

