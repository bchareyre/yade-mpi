/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __GLDRAWINTERACTIONSPHERE_HPP__
#define __GLDRAWINTERACTIONSPHERE_HPP__

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingSphere : public GLDrawInteractingGeometryFunctor
{	
	private :
		static bool first;
		static vector<Vector3r> vertices;
		static vector<Vector3r> faces;
		static int glWiredSphereList;
		static int glSphereList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawSphere(int depth);
	
	public :
		GLDrawInteractingSphere();
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool);
/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing){if(deserializing){first=true;};};

	RENDERS(InteractingSphere);
	REGISTER_CLASS_NAME(GLDrawInteractingSphere);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingSphere);

#endif //  GLDRAWINTERACTIONSPHERE_HPP

