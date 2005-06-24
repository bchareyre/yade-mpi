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

#include "Intersections2D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool segments2DIntersect(Vector2r& p1,Vector2r& p2,Vector2r& p3,Vector2r& p4)
{
        //restart = false;

        Vector2r n1 = Vector2r(p2[1]-p1[1],p1[0]-p2[0]);
        n1.normalize();
        Real o1 = n1.dot(p1);
        Vector2r n2 = Vector2r(p4[1]-p3[1],p3[0]-p4[0]);
        n2.normalize();
        Real o2 = n2.dot(p3);

        //if (fabs(n1.dot(n2))<Mathr::EPSILON) // approx angle between n1 and n2
        //{
                //restart = true;
                //return false;
        //}

        Real d1 = n1.dot(p3-o1*n1);
        Real d2 = n1.dot(p4-o1*n1);

        Real d3 = n2.dot(p1-o2*n2);
        Real d4 = n2.dot(p2-o2*n2);

        //if (fabs(d1)<Mathr::EPSILON || fabs(d2)<Mathr::EPSILON || fabs(d3)<Mathr::EPSILON || fabs(d4)<Mathr::EPSILON) // we consider that on point is on the other segment
        //{
                //restart = true;
        //      return false;
        //}
        //else
        return (d1*d2<0 && d3*d4<0); // if d1 and d2 don't have the same side ie are nont on the same side of the other segment

}


bool lines2DIntersection(Vector2r p1, Vector2r d1, Vector2r p2,Vector2r d2, bool& same, Vector2r& iPoint)
{
    // Intersection is a solution to P0+s*D0 = P1+t*D1.  Rewrite as
    // s*D0 - t*D1 = P1 - P0, a 2x2 system of equations.  If D0 = (x0,y0)
    // and D1 = (x1,y1) and P1 - P0 = (c0,c1), then the system is
    // x0*s - x1*t = c0 and y0*s - y1*t = c1.  The error tests are relative
    // to the size of the direction vectors, |Cross(D0,D1)| >= e*|D0|*|D1|
    // rather than absolute tests |Cross(D0,D1)| >= e.  The quantities
    // P1-P0, |D0|^2, and |D1|^2 are returned for use by calling functions.

    Real det = d2.dotPerp(d1);
    Vector2r v = p2 - p1;
    Real d1SqrL = d1.squaredLength();
        bool intersect = false;
        same=false;

    if ( det*det > Mathr::EPSILON*d1SqrL*d2.squaredLength() )
    {
        Real invDet = ((Real)1.0)/det;
                intersect = true;
        //s = d2.Kross(v)*invDet;
        //t = d1.Kross(v)*invDet;
                iPoint = p1+d1*d2.dotPerp(v)*invDet;
    }
    else // lines are parallel
    {
        det = d1.dotPerp(v);
        Real rhs = Mathr::EPSILON*d1SqrL*v.squaredLength();
        if ( det*det <= rhs )// lines are the same
        {
                        same = true;
                        intersect = true;
                }
    }

    return intersect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool segments2DIntersection(Vector2r p1,Vector2r p2, Vector2r a,Vector2r b, bool& same, Vector2r& iPoint)
{
    if (segments2DIntersect(p1,p2,a,b))
    {
                lines2DIntersection(p1, p2-p1, a ,b-a,same,iPoint);
                return true;
        }
        else
                return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool pointOnSegment2D(Vector2r& s1, Vector2r& s2, Vector2r& p, Real& c)
{
        if ( (s1-p).squaredLength()<Mathr::EPSILON)
        {
                c= 0;
                return true;
        }

        if ((s2-p).squaredLength()<Mathr::EPSILON)
        {
                c=1;
                return true;
        }

        Vector2r v1 = s2-s1;
        Vector2r v2 = p-s1;
        Real l1 = v1.length();
        Real l2 = v2.length();
        Real cosangle = v1.dot(v2)/(l1*l2);
        if (fabs(cosangle-1)<Mathr::EPSILON) // v1 and v2 colinear and in the the way
        {
                c =l2/l1;
                return (c>0 && c<1); // if p between s1 and s2
        }
        else
                return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int clipPolygon(Vector3r quad,const std::vector<Vector3r>& polygon, std::vector<Vector3r>& clipped)
{

	unsigned int v;
	int d;
	std::vector<Vector3r> tmpPoly;
	
	clipped.clear();
	//tmpPoly.clear();

	for (v=0; v<polygon.size(); v++)
	{
		d=(v+1)%polygon.size();
		clipLeft(quad[0],tmpPoly, polygon[v], polygon[d]);
	}
	
	for (v=0; v<tmpPoly.size(); v++)
	{
		d=(v+1)%tmpPoly.size();
		clipRight(quad[0],clipped, tmpPoly[v],tmpPoly[d]);
	}
	
	tmpPoly.clear();
	
	for (v=0; v<clipped.size(); v++)
	{
		d=(v+1)%clipped.size();
		clipTop(quad[1],tmpPoly, clipped[v],clipped[d]);
	}
	
	clipped.clear();
	
	for (v=0; v<tmpPoly.size(); v++)
	{
		d=(v+1)%tmpPoly.size();
		clipBottom(quad[1],clipped, tmpPoly[v],tmpPoly[d]);
	}
	
	return clipped.size();	
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void clipLeft(Real sizeX, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2)
{

	Real dx,dy,m;
	Vector3r v;
	
   
	dx = v2[0]-v1[0];
	dy = v2[1]-v1[1];
	
	if (dx!=0)
		m = dy/dx;
	else
		m = 1;
		
	if ( v1[0]>=-sizeX && v2[0]>=-sizeX )
		polygon.push_back(v2);
			  
	if ( v1[0]>=-sizeX && v2[0]<-sizeX )
	{
		v[0] = -sizeX;
		v[1] = v1[1]+m*(-sizeX-v1[0]);		
		polygon.push_back(v);
	}

	if ( v1[0]<-sizeX && v2[0]>=-sizeX )
	{
		v[0] = -sizeX;
		v[1] = v1[1]+m*(-sizeX-v1[0]);		
		polygon.push_back(v);
		polygon.push_back(v2);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void clipRight(Real sizeX, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2)
{
	
	Real dx,dy,m;
	Vector3r v;
	
   
	dx = v2[0]-v1[0];
	dy = v2[1]-v1[1];
	
	if (dx!=0)
		m = dy/dx;
	else
		m = 1;
   

	if ( v1[0]<=sizeX && v2[0]<=sizeX )
		polygon.push_back(v2);
			  
	if ( v1[0]<=sizeX && v2[0]>sizeX )
	{
		v[0] = sizeX;
		v[1] = v1[1]+m*(sizeX-v1[0]);		
		polygon.push_back(v);
	}

	if ( v1[0]>sizeX && v2[0]<=sizeX )
	{
		v[0] = sizeX;
		v[1] = v1[1]+m*(sizeX-v1[0]);		
		polygon.push_back(v);
		polygon.push_back(v2);
	}
   
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void clipTop(Real sizeY, std::vector<Vector3r>& polygon, Vector3r v1, Vector3r v2)
{
	
	Real dx,dy,m;
	Vector3r v;
	
	dx = v2[0]-v1[0];
	dy = v2[1]-v1[1];
	
	if (dx!=0)
		m = dy/dx;
	else
		m = 1;
		

	if ( v1[1]<=sizeY && v2[1]<=sizeY )
		polygon.push_back(v2);
			  
	if ( v1[1]<=sizeY && v2[1]>sizeY )
	{
		if (dx!=0)
			v[0] = v1[0]+(sizeY-v1[1])/m;			
		else
			v[0] = v1[0];
		v[1] = sizeY;
		polygon.push_back(v);
	}

	if ( v1[1]>sizeY && v2[1]<=sizeY )
	{
		if (dx!=0)
			v[0] = v1[0]+(sizeY-v1[1])/m;			
		else
			v[0] = v1[0];
		v[1] = sizeY;
		polygon.push_back(v);
		polygon.push_back(v2);
	}
		
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void clipBottom(Real sizeY, std::vector<Vector3r> &polygon, Vector3r v1, Vector3r v2)
{
	
	Real dx,dy,m;
	Vector3r v;
	   
	dx = v2[0]-v1[0];
	dy = v2[1]-v1[1];
	
	if (dx!=0)
		m = dy/dx;
	else
		m = 1;

	if ( v1[1]>=-sizeY && v2[1]>=-sizeY )
		polygon.push_back(v2);
			  
	if ( v1[1]>=-sizeY && v2[1]<-sizeY )
	{
		if (dx!=0)
			v[0] = v1[0]+(-sizeY-v1[1])/m;			
		else
			v[0] = v1[0];
		v[1] = -sizeY;
		polygon.push_back(v);
	}

	if ( v1[1]<-sizeY && v2[1]>=-sizeY )
	{
		if (dx!=0)
			v[0] = v1[0]+(-sizeY-v1[1])/m;			
		else
			v[0] = v1[0];
		v[1] = -sizeY;
		polygon.push_back(v);
		polygon.push_back(v2);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

