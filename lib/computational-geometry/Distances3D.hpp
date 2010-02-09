/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-base/Math.hpp>

#include <vector>

using namespace std;

Real sqrDistTriPoint(const Vector3r& p, const vector<Vector3r>& tri, Vector3r& pt);


