/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractingGeometry.hpp>

class InteractingNode : public InteractingGeometry
{
	public :
		InteractingNode ();
		virtual ~InteractingNode ();

/// Serialization
	REGISTER_ATTRIBUTES(InteractingGeometry,/* */);
	REGISTER_CLASS_NAME(InteractingNode);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

/// Indexable
	REGISTER_CLASS_INDEX(InteractingNode,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingNode);

