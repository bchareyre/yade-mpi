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

#include "Box2Box4ClosestFeatures.hpp"
#include "Box.hpp"
#include "ClosestFeatures.hpp"
#include "Math.hpp"
#include "Intersections2D.hpp"
#include "Intersections3D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Box4ClosestFeatures::go(		const shared_ptr<InteractionGeometry>& cm1,
						const shared_ptr<InteractionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{
	Real r11,r12,r13,r21,r22,r23,r31,r32,r33,q11,q12,q13,q21,q22,q23,q31,q32,q33;
	
	Matrix3r axis1,axis1T,axis2,axis2T;
	Vector3r _normal;
	Vector3r pt1, pt2, o1p1, o2p2;
	Vector3r extents1,extents2;
	
	shared_ptr<Box> obb1 = shared_dynamic_cast<Box>(cm1);
	shared_ptr<Box> obb2 = shared_dynamic_cast<Box>(cm2);

	int nbInteractions = 0;
	
	se31.rotation.toRotationMatrix(axis1T);
	axis1 = axis1T.transpose();
	
	se32.rotation.toRotationMatrix(axis2T);
	axis2 = axis2T.transpose();
		
	extents1 = obb1->extents;
	extents2 = obb2->extents;
	
	//translation, in parent frame
	Vector3r p = se32.translation-se31.translation;

	//translation of p in A's frame
	Vector3r pp;
	pp = axis1*p;

	//calculate rotation matrix			
	r11 = (axis1.getRow(0)).dot(axis2.getRow(0)); r12 = (axis1.getRow(0)).dot(axis2.getRow(1)); r13 = (axis1.getRow(0)).dot(axis2.getRow(2));
	r21 = (axis1.getRow(1)).dot(axis2.getRow(0)); r22 = (axis1.getRow(1)).dot(axis2.getRow(1)); r23 = (axis1.getRow(1)).dot(axis2.getRow(2));
	r31 = (axis1.getRow(2)).dot(axis2.getRow(0)); r32 = (axis1.getRow(2)).dot(axis2.getRow(1)); r33 = (axis1.getRow(2)).dot(axis2.getRow(2));
	/*Matrix3r R = axis1*axis2T;
	r11 = R[0][0]; r12 = R[0][1]; r13 = R[0][2];
	r21 = R[1][0]; r22 = R[1][1]; r23 = R[1][2];
	r31 = R[2][0]; r32 = R[2][1]; r33 = R[2][2];*/
		
	q11 = fabs(r11); q12 = fabs(r12); q13 = fabs(r13);
	q21 = fabs(r21); q22 = fabs(r22); q23 = fabs(r23);
	q31 = fabs(r31); q32 = fabs(r32); q33 = fabs(r33);

	BoxBoxInteractionInfo bbInfo;
	
	bbInfo.isNormalPrincipalAxis = true;
	bbInfo.invertNormal = false;
	bbInfo.penetrationDepth = -Mathr::MAX_REAL+1;
	bbInfo.code = 0;
	
	// separating axis = u1,u2,u3
	if (!testSeparatingAxis(pp[0],(extents1[0] + extents2[0]*q11 + extents2[1]*q12 + extents2[2]*q13),axis1.getRow(0),1,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[1],(extents1[1] + extents2[0]*q21 + extents2[1]*q22 + extents2[2]*q23),axis1.getRow(1),2,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[2],(extents1[2] + extents2[0]*q31 + extents2[1]*q32 + extents2[2]*q33),axis1.getRow(2),3,&bbInfo))
		return false;

	// separating axis = v1,v2,v3
	if (!testSeparatingAxis(axis2.getRow(0).dot(p),(extents1[0]*q11+extents1[1]*q21+extents1[2]*q31+extents2[0]),axis2.getRow(0),4,&bbInfo))
		return false;	

	if (!testSeparatingAxis(axis2.getRow(1).dot(p),(extents1[0]*q12+extents1[1]*q22+extents1[2]*q32+extents2[1]),axis2.getRow(1),5,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(axis2.getRow(2).dot(p),(extents1[0]*q13+extents1[1]*q23+extents1[2]*q33+extents2[2]),axis2.getRow(2),6,&bbInfo))
		return false;

	// separating axis = u1 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[2]*r21-pp[1]*r31,(extents1[1]*q31+extents1[2]*q21+extents2[1]*q13+extents2[2]*q12),Vector3r(0,-r31,r21),7,&bbInfo))
		return false;		
		
	if (!testSeparatingAxis(pp[2]*r22-pp[1]*r32,(extents1[1]*q32+extents1[2]*q22+extents2[0]*q13+extents2[2]*q11),Vector3r(0,-r32,r22),8,&bbInfo))
		return false;		
		
	if (!testSeparatingAxis(pp[2]*r23-pp[1]*r33,(extents1[1]*q33+extents1[2]*q23+extents2[0]*q12+extents2[1]*q11),Vector3r(0,-r33,r23),9,&bbInfo))
		return false;
		
	// separating axis = u2 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[0]*r31-pp[2]*r11,(extents1[0]*q31+extents1[2]*q11+extents2[1]*q23+extents2[2]*q22),Vector3r(r31,0,-r11),10,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[0]*r32-pp[2]*r12,(extents1[0]*q32+extents1[2]*q12+extents2[0]*q23+extents2[2]*q21),Vector3r(r32,0,-r12),11,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[0]*r33-pp[2]*r13,(extents1[0]*q33+extents1[2]*q13+extents2[0]*q22+extents2[1]*q21),Vector3r(r33,0,-r13),12,&bbInfo))
		return false;	

	// separating axis = u3 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[1]*r11-pp[0]*r21,(extents1[0]*q21+extents1[1]*q11+extents2[1]*q33+extents2[2]*q32),Vector3r(-r21,r11,0),13,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[1]*r12-pp[0]*r22,(extents1[0]*q22+extents1[1]*q12+extents2[0]*q33+extents2[2]*q31),Vector3r(-r22,r12,0),14,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[1]*r13-pp[0]*r23,(extents1[0]*q23+extents1[1]*q13+extents2[0]*q32+extents2[1]*q31),Vector3r(-r23,r13,0),15,&bbInfo))
		return false;

	int code = bbInfo.code;	
	
	if (code==0) return false;

	bool isNormalPrincipalAxis = bbInfo.isNormalPrincipalAxis;
	bool invertNormal = bbInfo.invertNormal;
	Real penetrationDepth = bbInfo.penetrationDepth;
	Vector3r normal = bbInfo.normal;
		
	if (!isNormalPrincipalAxis)	
		_normal = axis1T*normal;
	else	
		_normal = normal;
		
	if (invertNormal)
		normal = -_normal;
	else
		normal = _normal;
		
	normal.normalize(); 
	penetrationDepth = -penetrationDepth;

	if (code > 6) 
	{
		// an edge from box 1 touches an edge from box 2.
		// find a point pa on the intersecting edge of box 1
		Vector3r pa,pb;
		Vector3r sign;

		pa = se31.translation;
		sign[0] = (axis1.getRow(0).dot(normal) > 0) ? 1.0 : -1.0;
		sign[1] = (axis1.getRow(1).dot(normal) > 0) ? 1.0 : -1.0;
		sign[2] = (axis1.getRow(2).dot(normal) > 0) ? 1.0 : -1.0;
		pa+=(sign[0]*extents1[0])*axis1.getRow(0);
		pa+=(sign[1]*extents1[1])*axis1.getRow(1);
		pa+=(sign[2]*extents1[2])*axis1.getRow(2);

		// find a point pb on the intersecting edge of box 2
		pb = se32.translation;
		sign[0] = (axis2.getRow(0).dot(normal) > 0) ? -1.0 : 1.0;
		sign[1] = (axis2.getRow(1).dot(normal) > 0) ? -1.0 : 1.0;
		sign[2] = (axis2.getRow(2).dot(normal) > 0) ? -1.0 : 1.0;
		pb+=(sign[0]*extents2[0])*axis2.getRow(0);
		pb+=(sign[1]*extents2[1])*axis2.getRow(1);
		pb+=(sign[2]*extents2[2])*axis2.getRow(2);

		Real alpha,beta;
		Vector3r ua,ub;
		
		//ua = axis1[(code-7)/3];
		//ub = axis2[(code-7)%3];
		// FIXME : I'm not sure it is getRow now maybe it is getColumn
		ua = axis1.getRow((code-7)/3);
		ub = axis2.getRow((code-7)%3);
		
		lineClosestApproach(pa,ua,pb,ub,alpha,beta);
		pa+=alpha*ua;
		pb+=beta*ub;

		//pt1 = pa;//(pa+pb+normal*penetrationDepth)*0.5;
		//pt2 = pb;//(pa+pb-normal*penetrationDepth)*0.5;
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(pa,pb));
		c->interactionGeometry = cf;
		
		nbInteractions++;

		return true;	
	}
	
	
	Vector3r pa,pb,Sa,Sb;
	Matrix3r Ra,Rb;
	
	if (code <= 3) 
	{
		Ra = axis1;
		Rb = axis2;
		pa = se31.translation;
		pb = se32.translation;
		Sa = extents1;
		Sb = extents2;
	}
	else 
	{
		Ra = axis2;
		Rb = axis1;
		pa = se32.translation;
		pb = se31.translation;
		Sa = extents2;
		Sb = extents1;
	}

	// nr = normal vector of reference face dotted with axes of incident box.
	// anr = absolute values of nr.
	Vector3r normal2,nr,anr;
	if (code <= 3) 
		normal2 = normal;
	else 
		normal2 = -normal;
	
	nr = Rb*normal2;

	anr[0] = fabs(nr[0]);
	anr[1] = fabs(nr[1]);
	anr[2] = fabs(nr[2]);

	// find the largest compontent of anr: this corresponds to the normal
	// for the indident face. the other axis numbers of the indicent face
	// are stored in a1,a2.
	int lanr,a1,a2;
	if (anr[1] > anr[0]) 
	{	
		if (anr[1] > anr[2]) 
		{
			a1 = 0;
			lanr = 1;
			a2 = 2;
		}
		else 
		{
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}
	else 
	{
		if (anr[0] > anr[2]) 
		{
			lanr = 0;
			a1 = 1;
			a2 = 2;
		}
		else 
		{
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}

	// compute center point of incident face, in reference-face coordinates
	Vector3r center;
	if (nr[lanr] < 0) 
		center = pb - pa + Rb.getRow(lanr)*Sb[lanr];
	else 
		center = pb - pa - Rb.getRow(lanr)*Sb[lanr];

	// find the normal and non-normal axis numbers of the reference box
	int codeN,code1,code2;
	
	if (code <= 3) 
		codeN = code-1; 
	else 
		codeN = code-4;
	
	if (codeN==0) 
	{
		code1 = 1;
		code2 = 2;
	}
	else if (codeN==1) 
	{
		code1 = 0;
		code2 = 2;
	}
	else 
	{
		code1 = 0;
		code2 = 1;
	}


	// find the four corners of the incident face, in reference-face coordinates
	std::vector<Vector3r> quad;	// 2D coordinate of incident face (x,y pairs)
	Real c1,c2,m11,m12,m21,m22;
	//c1 = center.dot(Ra.code1]);
	//c2 = center.dot(Ra[code2]);
	c1 = center.dot(Ra.getRow(code1));
	c2 = center.dot(Ra.getRow(code2));
	
	// optimize this? - we have already computed this data above, but it is not
	// stored in an easy-to-index format. for now it's quicker just to recompute
	// the four dot products.
	m11 = Ra.getRow(code1).dot(Rb.getRow(a1));
	m12 = Ra.getRow(code1).dot(Rb.getRow(a2));
	m21 = Ra.getRow(code2).dot(Rb.getRow(a1));
	m22 = Ra.getRow(code2).dot(Rb.getRow(a2));
	
	Real k1,k2,k3,k4;
	
	k1 = m11*Sb[a1];
	k2 = m21*Sb[a1];
	k3 = m12*Sb[a2];
	k4 = m22*Sb[a2];

	quad.push_back(Vector3r(c1 - k1 - k3, c2 - k2 - k4,0));
	quad.push_back(Vector3r(c1 - k1 + k3, c2 - k2 + k4,0));
	quad.push_back(Vector3r(c1 + k1 + k3, c2 + k2 + k4,0));
	quad.push_back(Vector3r(c1 + k1 - k3, c2 + k2 - k4,0));

	// find the size of the reference face
	Vector3r rect;
	rect = Vector3r(Sa[code1],Sa[code2],0);

	// intersect the incident and reference faces
	std::vector<Vector3r> ret;

	int n = clipPolygon (rect,quad,ret);

	if (n < 1) 
		return false;	// this should never happen

	// convert the intersection points into reference-face coordinates,
	// and compute the contact position and depth for each point. only keep
	// those points that have a positive (penetrating) depth. delete points in
	// the 'ret' array as necessary so that 'point' and 'ret' correspond.
	std::vector<Vector3r> point;	// penetrating contact points
	std::vector<Real> dep;			// depths for those points

	Real det1 = 1.0/(m11*m22 - m12*m21);

	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;

	int cnum = 0;			// number of penetrating contact points found

	point.resize(8);
	dep.resize(8);
	int j;	
	for (j=0; j < n; j++) 
	{
		k1 =  m22*(ret[j][0]-c1) - m12*(ret[j][1]-c2);
		k2 = -m21*(ret[j][0]-c1) + m11*(ret[j][1]-c2);
		
		point[cnum] = center + k1*Rb.getRow(a1) + k2*Rb.getRow(a2);
		
		dep[cnum] = Sa[codeN] - normal2.dot(point[cnum]);
		
		if (dep[cnum] >= 0) 
		{
			ret[cnum][0] = ret[j][0];
			ret[cnum][1] = ret[j][1];
			cnum++;
		}
	}
	
	if (cnum < 1) 
		return false;	// this should never happen
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	for (j=0; j < cnum; j++) 
	{
		if (code<=3)
		{	
			pt2	= point[j] + pa;
			pt1 	= pt2+normal*dep[j];
			//pt1	= point[j] + pa;
			//pt2 	= pt1+normal*dep[j];
			cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(pt1,pt2));
		}
		else
		{
			//pt2		= point[j] + pa;
			//pt1 	= pt2-normal*dep[j];
			pt1		= point[j] + pa;
			pt2 	= pt1-normal*dep[j];
			cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(pt1,pt2));
		}
		nbInteractions++;
	}
	c->interactionGeometry = cf;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Box4ClosestFeatures::goReverse(	const shared_ptr<InteractionGeometry>& cm1,
						const shared_ptr<InteractionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		shared_ptr<ClosestFeatures> cf = shared_dynamic_cast<ClosestFeatures>(c->interactionGeometry);
		Vector3r tmp = cf->closestsPoints[0].first;
		cf->closestsPoints[0].first = cf->closestsPoints[0].second;		
		cf->closestsPoints[0].second = tmp;
	}
	return isInteracting;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Box4ClosestFeatures::testSeparatingAxis(Real expr1, Real expr2, Vector3r n,int c,BoxBoxInteractionInfo* bbInfo)
{
	Real pd,l;

	pd = fabs(expr1) - (expr2); 

	if (pd > 0) 
  		return false; 
		
	if (c>6)
	{		
		l = n.length();
		
		if (l>0)
		{
			pd /= l;
			if (pd > bbInfo->penetrationDepth) 
			{ 
				bbInfo->penetrationDepth = pd;
				bbInfo->normal = n;
				bbInfo->invertNormal = ((expr1) < 0);  
				bbInfo->code = c; 
				bbInfo->isNormalPrincipalAxis = false;
			}
		}
	}
	else
	{
		if (pd > bbInfo->penetrationDepth) 
		{ 
			bbInfo->penetrationDepth = pd; 
			bbInfo->normal = n;
			bbInfo->invertNormal = ((expr1) < 0);  
			bbInfo->code = c; 
			bbInfo->isNormalPrincipalAxis = true;
		}
	}
	
	return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
