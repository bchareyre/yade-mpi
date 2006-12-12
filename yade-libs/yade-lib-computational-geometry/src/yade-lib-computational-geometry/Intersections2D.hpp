/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERSECTION2D_HPP
#define INTERSECTION2D_HPP

#include <Wm3Math.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Vector2.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <vector>

using namespace std;
	
	bool segments2DIntersect(Vector2r& p1,Vector2r& p2,Vector2r& p3,Vector2r& p4);
	bool lines2DIntersection(Vector2r p1, Vector2r d1, Vector2r p2,Vector2r d2, bool& same, Vector2r& iPoint);
	bool segments2DIntersection(Vector2r p1, Vector2r d1, Vector2r p2,Vector2r d2, bool& same, Vector2r& iPoint);

	bool pointOnSegment2D(Vector2r& s1, Vector2r& s2, Vector2r& p, Real& c);
	
	int clipPolygon(Vector3r quad,const std::vector<Vector3r>& polygon, std::vector<Vector3r>& clipped);
	void clipLeft(Real sizeX, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2);
	void clipRight(Real sizeX, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2);
	void clipTop(Real sizeY, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2);
	void clipBottom(Real sizeY, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2);

#endif // INTERSECTION2D_HPP

