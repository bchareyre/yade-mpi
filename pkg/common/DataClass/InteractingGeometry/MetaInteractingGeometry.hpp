/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>

class MetaInteractingGeometry : public Shape
{
	public :
		MetaInteractingGeometry ();
		virtual ~MetaInteractingGeometry ();

/// Serialization
	REGISTER_CLASS_AND_BASE(MetaInteractingGeometry,Shape);
	// FIXME: if this class is registered, Scene then has "abstract" Shape, which fails in the functor (??)
	REGISTER_ATTRIBUTES(Shape,/* no attributes */);
/// Indexable
	REGISTER_CLASS_INDEX(MetaInteractingGeometry,Shape);
};

REGISTER_SERIALIZABLE(MetaInteractingGeometry);

