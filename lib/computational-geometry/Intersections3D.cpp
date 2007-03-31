/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Intersections3D.hpp"


void lineClosestApproach (const Vector3r pa, const Vector3r ua, const Vector3r pb, const Vector3r ub, Real &alpha, Real &beta)
{
	Vector3r p;
	
	p = pb - pa;

	Real uaub = ua.Dot(ub);
	Real q1 =  ua.Dot(p);
	Real q2 = -ub.Dot(p);
	Real d = 1-uaub*uaub;
	
	if (d <= 0) 
	{
		// @@@ this needs to be made more robust
		alpha = 0;
		beta  = 0;
	}
	else 
	{
		d = 1/d;
		alpha = (q1 + uaub*q2)*d;
		beta  = (uaub*q1 + q2)*d;
	}
	
}

