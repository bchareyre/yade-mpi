/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Distances3D.hpp"

Real sqrDistTriPoint(const Vector3r& p, const vector<Vector3r>& tri, Vector3r& pt)
{
	Vector3r kDiff = tri[0] - p;
	Vector3r edge0 = tri[1]-tri[0];
	Vector3r edge1 = tri[2]-tri[0];
	Real fA00 = edge0.SquaredLength();
	Real fA01 = edge0.Dot(edge1);
	Real fA11 = edge1.SquaredLength();
	Real fB0 = kDiff.Dot(edge0);
	Real fB1 = kDiff.Dot(edge1);
	Real fC = kDiff.SquaredLength();
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

