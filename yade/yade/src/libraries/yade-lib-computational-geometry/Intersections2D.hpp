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

#ifndef __INTERSECTION2D_H__
#define __INTERSECTION2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-wm3-math/Vector3.hpp>
#include <vector>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	int clipPolygon(Vector3r quad,const std::vector<Vector3r>& polygon, std::vector<Vector3r>& clipped);
	void clipLeft(Real sizeX, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2);
	void clipRight(Real sizeX, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2);
	void clipTop(Real sizeY, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2);
	void clipBottom(Real sizeY, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INTERSECTION2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
