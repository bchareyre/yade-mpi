/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTING_NODE_HPP
#define INTERACTING_NODE_HPP

#include<yade/core/InteractingGeometry.hpp>

class InteractingNode : public InteractingGeometry
{
	public :
		InteractingNode ();
		virtual ~InteractingNode ();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(InteractingNode);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

/// Indexable
	REGISTER_CLASS_INDEX(InteractingNode,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingNode,false);

#endif // INTERACTING_NODE_HPP
