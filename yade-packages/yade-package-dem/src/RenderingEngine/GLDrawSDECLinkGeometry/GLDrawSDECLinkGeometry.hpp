/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_SPHERES_LINK_CONTACT_GEOMETRY_HPP
#define GLDRAW_SPHERES_LINK_CONTACT_GEOMETRY_HPP

#include <yade/yade-package-common/GLDrawInteractionGeometryFunctor.hpp>

class GLDrawSDECLinkGeometry : public GLDrawInteractionGeometryFunctor
{	
	private :
		Real midMax;
		Real forceMax;
	public :
		GLDrawSDECLinkGeometry();
		virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);

	RENDERS(SDECLinkGeometry);
	REGISTER_CLASS_NAME(GLDrawSDECLinkGeometry);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawSDECLinkGeometry,false);

#endif 

