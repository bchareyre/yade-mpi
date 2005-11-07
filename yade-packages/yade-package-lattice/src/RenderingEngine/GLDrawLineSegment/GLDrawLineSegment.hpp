/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_LINE_SEGMENT_HPP
#define GLDRAW_LINE_SEGMENT_HPP

#include <yade/yade-package-common/GLDrawGeometricalModelFunctor.hpp>

class GLDrawLineSegment : public GLDrawGeometricalModelFunctor
{
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);

	REGISTER_CLASS_NAME(GLDrawLineSegment);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);

};

REGISTER_SERIALIZABLE(GLDrawLineSegment,false);

#endif // GLDRAW_LINE_SEGMENT_HPP

