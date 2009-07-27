/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionOfMyTetrahedron.hpp"


InteractionOfMyTetrahedron::InteractionOfMyTetrahedron() : InteractionGeometry()
{
	createIndex();
}


InteractionOfMyTetrahedron::~InteractionOfMyTetrahedron ()
{
}

YADE_PLUGIN("InteractionOfMyTetrahedron");