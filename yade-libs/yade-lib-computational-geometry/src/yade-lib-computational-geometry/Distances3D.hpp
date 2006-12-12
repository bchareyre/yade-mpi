/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DISTANCES3D_HPP
#define DISTANCES3D_HPP

#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

#include <vector>

using namespace std;

Real sqrDistTriPoint(const Vector3r& p, const vector<Vector3r>& tri, Vector3r& pt);

#endif // __DISTANCES3D_H__

