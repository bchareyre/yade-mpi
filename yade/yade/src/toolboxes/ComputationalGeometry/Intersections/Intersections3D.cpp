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

#include "Intersections3D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void lineClosestApproach (const Vector3 pa, const Vector3 ua, const Vector3 pb, const Vector3 ub, float &alpha, float &beta)
{
	Vector3 p;
	
	p = pb - pa;

	float uaub = ua.dot(ub);
	float q1 =  ua.dot(p);
	float q2 = -ub.dot(p);
	float d = 1-uaub*uaub;
	
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
