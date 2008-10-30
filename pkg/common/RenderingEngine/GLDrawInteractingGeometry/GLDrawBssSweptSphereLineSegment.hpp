/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP
#define GLDRAW_BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawBssSweptSphereLineSegment : public GLDrawInteractingGeometryFunctor
{	
	private :
		static bool first;
		static int  glWiredList;
		static int  glList;
	
	public :
                GLDrawBssSweptSphereLineSegment();
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool);
/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing){if(deserializing){first=true;};};

        RENDERS(BssSweptSphereLineSegment);
        REGISTER_CLASS_NAME(GLDrawBssSweptSphereLineSegment);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawBssSweptSphereLineSegment,false);

#endif //  GLDRAW_BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP

