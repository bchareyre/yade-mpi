/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Facet.hpp"

Facet::Facet () : GeometricalModel()
{
	createIndex();
}


Facet::~Facet ()
{
}

YADE_PLUGIN();
