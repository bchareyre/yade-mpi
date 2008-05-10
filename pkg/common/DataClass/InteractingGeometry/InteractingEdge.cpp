/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "InteractingEdge.hpp"

InteractingEdge::InteractingEdge() : InteractingGeometry()
{
    createIndex();
}

InteractingEdge::~InteractingEdge()
{
}

void InteractingEdge::registerAttributes()
{
    InteractingGeometry::registerAttributes();
    REGISTER_ATTRIBUTE(edge);
    REGISTER_ATTRIBUTE(both);
    REGISTER_ATTRIBUTE(flat);
    REGISTER_ATTRIBUTE(normal1);
    REGISTER_ATTRIBUTE(normal2);
}

void InteractingEdge::postProcessAttributes(bool deserializing)
{
    if (deserializing)
	squaredLength = edge.SquaredLength();
}


YADE_PLUGIN();
