/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LINE_SEGMENT_HPP
#define LINE_SEGMENT_HPP


#include <yade/yade-core/GeometricalModel.hpp>


class LineSegment : public GeometricalModel
{
	public :
		Real length;
		LineSegment ();
		virtual ~LineSegment ();
	
/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(LineSegment);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	
/// Indexable
	REGISTER_CLASS_INDEX(LineSegment,GeometricalModel);

};

REGISTER_SERIALIZABLE(LineSegment,false);

#endif // LINE_SEGMENT_HPP

