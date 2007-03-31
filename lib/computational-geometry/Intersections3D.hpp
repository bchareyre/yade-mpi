/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERSECTION3D_HPP
#define INTERSECTION3D_HPP

#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

	void lineClosestApproach (const Vector3r pa, const Vector3r ua, const Vector3r pb, const Vector3r ub, Real &alpha, Real &beta);

#endif // INTERSECTION3D_HPP

