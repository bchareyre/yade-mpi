/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DISTANCES2D_HPP
#define DISTANCES2D_HPP

#include <Wm3Vector2.h>
#include<yade/lib-base/yadeWm3.hpp>

#include <vector>

using namespace std;

Real distancePointLine2D(Vector2r p, Vector2r o,Vector2r d);
Real distancePointSegment2D(Vector2r p, Vector2r x1,Vector2r x2);

#endif // DISTANCES2D_HPP

