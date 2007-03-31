/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Distances2D.hpp"
#include "Intersections2D.hpp"


Real distancePointLine2D(Vector2r p, Vector2r o,Vector2r d)
{
	Vector2r n = Vector2r(-d[1],d[0]);
	n.Normalize();
	Real d0 = o.Dot(n);
	Real d1 = p.Dot(n)-d0;
	return d1;

}

Real distancePointSegment2D(Vector2r p, Vector2r x1,Vector2r x2)
{
	Real d = distancePointLine2D(p, x1, x2-x1);
	Vector2r n = Vector2r(x1[1]-x2[1],x2[0]-x1[0]);
	n.Normalize();
	Vector2r pOn = p-d*n;
	Real c;
	if (!pointOnSegment2D(x1, x2, pOn, c))
	{
		d = (p-x1).Length();
		Real d2 = (p-x2).Length();
		if (d2<d)
			d = d2;
	}
	return d;
}


