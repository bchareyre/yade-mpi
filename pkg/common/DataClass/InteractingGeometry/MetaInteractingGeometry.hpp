/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractingGeometry.hpp>

class MetaInteractingGeometry : public InteractingGeometry
{
	public :
		MetaInteractingGeometry ();
		virtual ~MetaInteractingGeometry ();

/// Serialization
	REGISTER_CLASS_AND_BASE(MetaInteractingGeometry,InteractingGeometry);
	// FIXME: if this class is registered, MetaBody then has "abstract" InteractingGeometry, which fails in the functor (??)
	REGISTER_ATTRIBUTES(InteractingGeometry,/* no attributes */);
/// Indexable
	REGISTER_CLASS_INDEX(MetaInteractingGeometry,InteractingGeometry);
};

REGISTER_SERIALIZABLE(MetaInteractingGeometry);

