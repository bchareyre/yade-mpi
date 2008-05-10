/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "Vertex.hpp"

Vertex::Vertex () : GeometricalModel()
{
	createIndex();
}


Vertex::~Vertex ()
{
}


void Vertex::registerAttributes()
{
	GeometricalModel::registerAttributes();
}

YADE_PLUGIN();
