/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingNode.hpp"

InteractingNode::InteractingNode () : InteractingGeometry()
{
	createIndex();
}

InteractingNode::~InteractingNode ()
{
}

void InteractingNode::registerAttributes()
{
	InteractingGeometry::registerAttributes();
}


YADE_PLUGIN("InteractingNode");
