/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "BoxModel.hpp"
BoxModel::BoxModel () : GeometricalModel() { createIndex(); }
BoxModel::~BoxModel (){}
YADE_PLUGIN((BoxModel));

YADE_REQUIRE_FEATURE(geometricalmodel);


YADE_REQUIRE_FEATURE(PHYSPAR);

