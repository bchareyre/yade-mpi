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

#include "Distances3D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Real sqrDistTriPoint(const Vector3r& p, const vector<Vector3r>& tri, Vector3r& pt)
{
	Vector3r kDiff = tri[0] - p;
	Vector3r edge0 = tri[1]-tri[0];
	Vector3r edge1 = tri[2]-tri[0];
	Real fA00 = edge0.squaredLength();
	Real fA01 = edge0.dot(edge1);
	Real fA11 = edge1.squaredLength();
	Real fB0 = kDiff.dot(edge0);
	Real fB1 = kDiff.dot(edge1);
	Real fC = kDiff.squaredLength();
	Real fDet = fabs(fA00*fA11-fA01*fA01);
	Real fS = fA01*fB1-fA11*fB0;
	Real fT = fA01*fB0-fA00*fB1;
	Real fSqrDist;

	if ( fS + fT <= fDet )
	{
		if ( fS < (Real)0.0 )
		{
			if ( fT < (Real)0.0 )  // region 4
			{
				if ( fB0 < (Real)0.0 )
				{
					fT = (Real)0.0;
					if ( -fB0 >= fA00 )
					{
						fS = (Real)1.0;
						fSqrDist = fA00+((Real)2.0)*fB0+fC;
					}
					else
					{
						fS = -fB0/fA00;
						fSqrDist = fB0*fS+fC;
					}
				}
				else
				{
					fS = (Real)0.0;
					if ( fB1 >= (Real)0.0 )
					{
						fT = (Real)0.0;
						fSqrDist = fC;
					}
					else if ( -fB1 >= fA11 )
					{
						fT = (Real)1.0;
						fSqrDist = fA11+((Real)2.0)*fB1+fC;
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
				fS = (Real)0.0;
				if ( fB1 >= (Real)0.0 )
				{
					fT = (Real)0.0;
					fSqrDist = fC;
				}
				else if ( -fB1 >= fA11 )
				{
					fT = (Real)1.0;
					fSqrDist = fA11+((Real)2.0)*fB1+fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
		                }
			}
		}
		else if ( fT < (Real)0.0 )  // region 5
		{
			fT = (Real)0.0;
			if ( fB0 >= (Real)0.0 )
			{
				fS = (Real)0.0;
				fSqrDist = fC;
			}
			else if ( -fB0 >= fA00 )
			{
				fS = (Real)1.0;
				fSqrDist = fA00+((Real)2.0)*fB0+fC;
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
			Real fInvDet = ((Real)1.0)/fDet;
			fS *= fInvDet;
			fT *= fInvDet;
			fSqrDist = fS*(fA00*fS+fA01*fT+((Real)2.0)*fB0) +
			fT*(fA01*fS+fA11*fT+((Real)2.0)*fB1)+fC;
		}
	}
	else
	{
		Real fTmp0, fTmp1, fNumer, fDenom;
		if ( fS < (Real)0.0 )  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fS = (Real)1.0;
					fT = (Real)0.0;
					fSqrDist = fA00+((Real)2.0)*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = (Real)1.0 - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
					fT*(fA01*fS+fA11*fT+((Real)2.0)*fB1)+fC;
				}
			}
			else
			{
				fS = (Real)0.0;
				if ( fTmp1 <= (Real)0.0 )
				{
					fT = (Real)1.0;
					fSqrDist = fA11+((Real)2.0)*fB1+fC;
				}
				else if ( fB1 >= (Real)0.0 )
				{
					fT = (Real)0.0;
					fSqrDist = fC;
				}
				else
				{
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
				}
			}
		}
		else if ( fT < (Real)0.0 )  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if ( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-((Real)2.0)*fA01+fA11;
				if ( fNumer >= fDenom )
				{
					fT = (Real)1.0;
					fS = (Real)0.0;
					fSqrDist = fA11+((Real)2.0)*fB1+fC;
				}
				else
				{
					fT = fNumer/fDenom;
					fS = (Real)1.0 - fT;
					fSqrDist = fS*(fA00*fS+fA01*fT+((Real)2.0)*fB0) + fT*(fA01*fS+fA11*fT+((Real)2.0)*fB1)+fC;
				}
			}
			else
			{
				fT = (Real)0.0;
				if ( fTmp1 <= (Real)0.0 )
				{
					fS = (Real)1.0;
					fSqrDist = fA00+((Real)2.0)*fB0+fC;
				}
				else if ( fB0 >= (Real)0.0 )
				{
					fS = (Real)0.0;
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
			if ( fNumer <= (Real)0.0 )
			{
				fS = (Real)0.0;
				fT = (Real)1.0;
				fSqrDist = fA11+((Real)2.0)*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if ( fNumer >= fDenom )
                		{
					fS = (Real)1.0;
					fT = (Real)0.0;
					fSqrDist = fA00+((Real)2.0)*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = (Real)1.0 - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+((Real)2.0)*fB0) + fT*(fA01*fS+fA11*fT+((Real)2.0)*fB1)+fC;
				}
			}
		}
	}

	pt = tri[0]+fS*edge0+fT*edge1;
		
	return fabs(fSqrDist);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
