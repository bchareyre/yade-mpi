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

#include <yade-lib-computational-geometry/Intersections2D.hpp>

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

