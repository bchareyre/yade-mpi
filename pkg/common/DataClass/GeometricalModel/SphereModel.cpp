/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SphereModel.hpp"

YADE_REQUIRE_FEATURE(geometricalmodel);

SphereModel::SphereModel () : GeometricalModel()
{
	createIndex();
}


SphereModel::~SphereModel ()
{
}


YADE_PLUGIN((SphereModel));


YADE_REQUIRE_FEATURE(PHYSPAR);

