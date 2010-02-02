/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "FacetModel.hpp"
YADE_REQUIRE_FEATURE(geometricalmodel);

FacetModel::FacetModel () : GeometricalModel()
{
	createIndex();
}


FacetModel::~FacetModel ()
{
}

YADE_PLUGIN((FacetModel));


YADE_REQUIRE_FEATURE(PHYSPAR);

