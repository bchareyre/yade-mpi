/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWSPHERE_HPP
#define GLDRAWSPHERE_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawSphere : public GLDrawGeometricalModelFunctor
{
	private :
		static bool first;
		static vector<Vector3r> vertices;
		static vector<Vector3r> faces;
		static int glWiredSphereList;
		static int glSphereList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawSphere(int depth);
		void drawCircle(bool filled);
		void clearGlMatrix();
		static bool glutUse, glutNormalize;
		static int glutSlices, glutStacks;
		
	public :
		GLDrawSphere();
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);
		virtual void initgl(){first=true;};
/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing){if(deserializing){first=true;};};
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(glutUse); REGISTER_ATTRIBUTE(glutNormalize); REGISTER_ATTRIBUTE(glutSlices); REGISTER_ATTRIBUTE(glutStacks); }
		
	RENDERS(Sphere);
	REGISTER_CLASS_NAME(GLDrawSphere);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};
REGISTER_SERIALIZABLE(GLDrawSphere);

#endif // GLDRAWSPHERE_HPP

