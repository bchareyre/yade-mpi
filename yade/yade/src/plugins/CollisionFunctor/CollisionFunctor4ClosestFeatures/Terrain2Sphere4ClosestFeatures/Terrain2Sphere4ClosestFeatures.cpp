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

#include "Terrain2Sphere4ClosestFeatures.hpp"
#include "Sphere.hpp"
#include "Terrain.hpp"
#include "ClosestFeatures.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


Terrain2Sphere4ClosestFeatures::Terrain2Sphere4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Terrain2Sphere4ClosestFeatures::~Terrain2Sphere4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain2Sphere4ClosestFeatures::collide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	Vector3 pt;
	std::vector<Vector3> tri;
	bool inCollision =  false;
	std::vector<int> faces;
	
	shared_ptr<Terrain> t = shared_dynamic_cast<Terrain>(cm1);
	shared_ptr<Sphere> s  = shared_dynamic_cast<Sphere>(cm2);
	
	Vector3 min,max;
	Vector3 radius = Vector3(s->radius,s->radius,s->radius);
	min = se32.translation-radius;
	max = se32.translation+radius;
	
	t->getFaces(AABB((max-min)*0.5,(min+max)*0.5),faces);
	tri.resize(3);
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	
	for(unsigned int i=0;i<faces.size();i++)
	{
		int faceId = faces[i];
		tri[0] = t->vertices[t->faces[faceId][0]]+se31.translation;
		tri[1] = t->vertices[t->faces[faceId][1]]+se31.translation;
		tri[2] = t->vertices[t->faces[faceId][2]]+se31.translation;
		float d = sqrDistTriPoint(se32.translation, tri, pt);
				
		if (d<s->radius*s->radius)
		{
			Vector3 v = pt-se32.translation;
			v.unitize();
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt,se32.translation+v*s->radius));
			inCollision =  true;
		}
	}
		
	if (inCollision)
		c->contactModel = cf;

	return inCollision;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain2Sphere4ClosestFeatures::reverseCollide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	bool isColliding = collide(cm2,cm1,se32,se31,c);
	if (isColliding)
	{
		shared_ptr<ClosestFeatures> cf = shared_dynamic_cast<ClosestFeatures>(c->contactModel);
		Vector3 tmp = cf->closestsPoints[0].first;
		cf->closestsPoints[0].first = cf->closestsPoints[0].second;		
		cf->closestsPoints[0].second = tmp;
	}
	return isColliding;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


float Terrain2Sphere4ClosestFeatures::sqrDistTriPoint(const Vector3& p, const std::vector<Vector3>& tri, Vector3& pt)
{
	Vector3 kDiff = tri[0] - p;
	Vector3 edge0 = tri[1]-tri[0];
	Vector3 edge1 = tri[2]-tri[0];
	float fA00 = edge0.squaredLength();
	float fA01 = edge0.dot(edge1);
	float fA11 = edge1.squaredLength();
	float fB0 = kDiff.dot(edge0);
	float fB1 = kDiff.dot(edge1);
	float fC = kDiff.squaredLength();
	float fDet = fabs(fA00*fA11-fA01*fA01);
	float fS = fA01*fB1-fA11*fB0;
	float fT = fA01*fB0-fA00*fB1;
	float fSqrDist;

	if ( fS + fT <= fDet )
	{
		if ( fS < (float)0.0 )
		{
			if ( fT < (float)0.0 )  // region 4
			{
				if ( fB0 < (float)0.0 )
				{
					fT = (float)0.0;
					if ( -fB0 >= fA00 )
					{
						fS = (float)1.0;
						fSqrDist = fA00+((float)2.0)*fB0+fC;
					}
					else
					{
						fS = -fB0/fA00;
						fSqrDist = fB0*fS+fC;
					}
				}
				else
				{
					fS = (float)0.0;
					if ( fB1 >= (float)0.0 )
					{
						fT = (float)0.0;
						fSqrDist = fC;
					}
					else if ( -fB1 >= fA11 )
					{
						fT = (float)1.0;
						fSqrDist = fA11+((float)2.0)*fB1+fC;
					}
					else
					{
						fT = -fB1/fA11;
						fSqrDist = fB1*fT+fC;
					}
				}
			}
			else  // region 3
			{
				fS = (float)0.0;
				if ( fB1 >= (float)0.0 )
				{
					fT = (float)0.0;
					fSqrDist = fC;
				}
				else if ( -fB1 >= fA11 )
				{
					fT = (float)1.0;
					fSqrDist = fA11+((float)2.0)*fB1+fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
		                }
			}
		}
		else if ( fT < (float)0.0 )  // region 5
		{
			fT = (float)0.0;
			if ( fB0 >= (float)0.0 )
			{
				fS = (float)0.0;
				fSqrDist = fC;
			}
			else if ( -fB0 >= fA00 )
			{
				fS = (float)1.0;
				fSqrDist = fA00+((float)2.0)*fB0+fC;
			}
			else
			{
				fS = -fB0/fA00;
				fSqrDist = fB0*fS+fC;
			}
		}
		else  // region 0
		{
			// minimum at interior point
			float fInvDet = ((float)1.0)/fDet;
			fS *= fInvDet;
			fT *= fInvDet;
			fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
			fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
		}
	}
	else
	{
		float fTmp0, fTmp1, fNumer, fDenom;
		if ( fS < (float)0.0 )  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fS = (float)1.0;
					fT = (float)0.0;
					fSqrDist = fA00+((float)2.0)*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = (float)1.0 - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
					fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
				}
			}
			else
			{
				fS = (float)0.0;
				if ( fTmp1 <= (float)0.0 )
				{
					fT = (float)1.0;
					fSqrDist = fA11+((float)2.0)*fB1+fC;
				}
				else if ( fB1 >= (float)0.0 )
				{
					fT = (float)0.0;
					fSqrDist = fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
				}
			}
		}
		else if ( fT < (float)0.0 )  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-((float)2.0)*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fT = (float)1.0;
					fS = (float)0.0;
					fSqrDist = fA11+((float)2.0)*fB1+fC;
				}
				else
				{
					fT = fNumer/fDenom;
					fS = (float)1.0 - fT;
					fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) + fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
				}
			}
			else
			{
				fT = (float)0.0;
				if ( fTmp1 <= (float)0.0 )
				{
					fS = (float)1.0;
					fSqrDist = fA00+((float)2.0)*fB0+fC;
				}
				else if ( fB0 >= (float)0.0 )
				{
					fS = (float)0.0;
					fSqrDist = fC;
				}
				else
				{
					fS = -fB0/fA00;
					fSqrDist = fB0*fS+fC;
				}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if ( fNumer <= (float)0.0 )
			{
				fS = (float)0.0;
				fT = (float)1.0;
				fSqrDist = fA11+((float)2.0)*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
                		{
					fS = (float)1.0;
					fT = (float)0.0;
					fSqrDist = fA00+((float)2.0)*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = (float)1.0 - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) + fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
				}
			}
		}
	}

	pt = tri[0]+fS*edge0+fT*edge1;
		
	return fabs(fSqrDist);
}
