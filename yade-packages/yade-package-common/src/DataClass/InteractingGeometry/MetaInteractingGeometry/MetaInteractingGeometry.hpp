/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METAINTERACTINGGEOMETRY_HPP
#define METAINTERACTINGGEOMETRY_HPP

#include <yade/yade-core/InteractingGeometry.hpp>

class MetaInteractingGeometry : public InteractingGeometry
{
	public :
		MetaInteractingGeometry ();
		virtual ~MetaInteractingGeometry ();

/// Serialization
	REGISTER_CLASS_NAME(MetaInteractingGeometry);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);
	
/// Indexable
	REGISTER_CLASS_INDEX(MetaInteractingGeometry,InteractingGeometry);
};

REGISTER_SERIALIZABLE(MetaInteractingGeometry,false);

#endif // METAINTERACTINGGEOMETRY_HPP

