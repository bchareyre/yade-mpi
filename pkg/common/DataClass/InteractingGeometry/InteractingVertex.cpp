/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "InteractingVertex.hpp"

InteractingVertex::InteractingVertex() : InteractingGeometry()
{
    createIndex();
}

InteractingVertex::~InteractingVertex()
{
}

void InteractingVertex::registerAttributes()
{
    InteractingGeometry::registerAttributes();
    REGISTER_ATTRIBUTE(normals);
    REGISTER_ATTRIBUTE(flat);
}

YADE_PLUGIN();
