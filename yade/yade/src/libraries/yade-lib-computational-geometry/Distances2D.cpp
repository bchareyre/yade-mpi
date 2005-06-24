/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Distances2D.hpp"
#include "Intersections2D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Real distancePointLine2D(Vector2r p, Vector2r o,Vector2r d)
{
	Vector2r n = Vector2r(-d[1],d[0]);
	n.normalize();
	Real d0 = o.dot(n);
	Real d1 = p.dot(n)-d0;
	return d1;

}

Real distancePointSegment2D(Vector2r p, Vector2r x1,Vector2r x2)
{
	Real d = distancePointLine2D(p, x1, x2-x1);
	Vector2r n = Vector2r(x1[1]-x2[1],x2[0]-x1[0]);
	n.normalize();
	Vector2r pOn = p-d*n;
	Real c;
	if (!pointOnSegment2D(x1, x2, pOn, c))
	{
		d = (p-x1).length();
		Real d2 = (p-x2).length();
		if (d2<d)
			d = d2;
	}
	return d;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
