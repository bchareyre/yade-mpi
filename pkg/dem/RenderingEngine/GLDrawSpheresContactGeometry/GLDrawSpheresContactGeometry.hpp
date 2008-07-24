/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_SPHERES_CONTACT_GEOMETRY_HPP
#define GLDRAW_SPHERES_CONTACT_GEOMETRY_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawSpheresContactGeometry : public GLDrawInteractionGeometryFunctor
{	
	private :
		Real midMax;
		Real forceMax;
	public :
		GLDrawSpheresContactGeometry();
		virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);

	RENDERS(SpheresContactGeometry);
	REGISTER_CLASS_NAME(GLDrawSpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawSpheresContactGeometry,false);

#endif 

