#include "BVCFFinalCollider.hpp"

#include "Sphere.hpp"
#include "Box.hpp"
#include "Terrain.hpp"
#include "ClosestFeatures.hpp"


BVCFFinalCollider::BVCFFinalCollider() : FinalCollider()
{	
	addPair((new Sphere())->type,(new Sphere())->type, &collideSphereSphere,NULL);
	addPair((new Sphere())->type,(new Box())->type, &collideSphereBox,&collideBoxSphere);
	addPair((new Box())->type,(new Box())->type, &collideBoxBox,NULL);
	addPair((new Terrain())->type,(new Sphere())->type, &collideTerrainSphere, &collideSphereTerrain);
	addPair((new Terrain())->type,(new Box())->type, &collideTerrainSphere, &collideSphereTerrain);
	addPair((new Box())->type,(new Terrain())->type, &collideTerrainBox, &collideBoxTerrain);
}

BVCFFinalCollider::~BVCFFinalCollider ()
{

}

void BVCFFinalCollider::processAttributes()
{

}

void BVCFFinalCollider::registerAttributes()
{
}
	

bool BVCFFinalCollider::collideSphereSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	shared_ptr<Sphere> s1 = shared_dynamic_cast<Sphere>(cm1);
	shared_ptr<Sphere> s2 = shared_dynamic_cast<Sphere>(cm2);
	
	Vector3 v = se31.translation-se32.translation;
	float l = v.unitize();
	
	if (l<s1->radius+s2->radius)
	{
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(se31.translation-v*s1->radius,se32.translation+v*s2->radius));
		c->contactModel = cf;
		return true;
	}
	else	
		return false;
}

bool BVCFFinalCollider::collideAABoxSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32,  shared_ptr<Contact> c)
{
	
	Vector3 l,t,p,q,r;
	bool onborder = false;
	Vector3 pt1,pt2,normal;
	float depth;
	
	shared_ptr<Sphere> s = shared_dynamic_cast<Sphere>(cm2);
	shared_ptr<Box>  aaBox = shared_dynamic_cast<Box>(cm1);
	
	p = se32.translation-se31.translation;
	
	l[0] = aaBox->extents[0];
	t[0] = p[0]; 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = aaBox->extents[1];
	t[1] = p[1];
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = aaBox->extents[2];
	t[2] = p[2];
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }

	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		float min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			float tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3 tmp = Vector3(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = tmp;
		
		normal.unitize();
		
		pt1 = se32.translation + normal*min;
		pt2 = se32.translation - normal*s->radius;
	
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
		c->contactModel = cf;
		
		return true;
	}

	q = t;
	
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0)
		return false;
	
	pt1 = q + se31.translation;

	normal = r;
	normal.unitize();

	pt2 = se32.translation - normal * s->radius;
		
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
	c->contactModel = cf;
	
	return true;	
}


bool BVCFFinalCollider::collideSphereAABox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32,  shared_ptr<Contact> c)
{
	Vector3 l,t,p,q,r;
	bool onborder = false;
	Vector3 pt1,pt2,normal;
	float depth;
	
	shared_ptr<Sphere> s = shared_dynamic_cast<Sphere>(cm1);
	shared_ptr<Box> aabox = shared_dynamic_cast<Box>(cm2);
	
	p = se31.translation-se32.translation;
	
	l[0] = aabox->extents[0];
	t[0] = p[0]; 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = aabox->extents[1];
	t[1] = p[1];
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = aabox->extents[2];
	t[2] = p[2];
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }

	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		float min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			float tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3 tmp = Vector3(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = tmp;
		
		normal.unitize();
		
		pt1 = se31.translation + normal*min;
		pt2 = se31.translation - normal*s->radius;
		
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt2,pt1));
		c->contactModel = cf;
		
		return true;
	}

	q = t;
	
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0)
		return false;
	
	pt1 = q + se32.translation;

	normal = r;
	normal.unitize();

	pt2 = se31.translation - normal * s->radius;
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt2,pt1));
	c->contactModel = cf;
	
	return true;	
}

bool BVCFFinalCollider::collideSphereBox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	if (se32.rotation == Quaternion())
		return collideSphereAABox(cm1,cm2,se31,se32,c);
		
	Vector3 l,t,p,q,r;
	bool onborder = false;
	Vector3 pt1,pt2,normal;
	Matrix3 axisT,axis;
	float depth;

	shared_ptr<Sphere> s = shared_dynamic_cast<Sphere>(cm1);
	shared_ptr<Box> obb = shared_dynamic_cast<Box>(cm2);
	
	Vector3 extents = obb->extents;
	
	se32.rotation.toRotationMatrix(axisT);
	axis = axisT.Transpose();
	
	p = se31.translation-se32.translation;
	
	l[0] = extents[0];
	t[0] = axis.GetRow(0).dot(p); 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = extents[1];
	t[1] = axis.GetRow(1).dot(p);
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = extents[2];
	t[2] = axis.GetRow(2).dot(p);
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }
	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		float min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			float tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3 tmp = Vector3(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = axisT*tmp;
		
		normal.unitize();
		
		pt1 = se31.translation + normal*min;
		pt2 = se31.translation - normal*s->radius;	
			
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt2,pt1));
		c->contactModel = cf;
	
		return true;	
	}

	q = axisT*t;
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0) 
		return false;

	pt1 = q + se32.translation;

	normal = r;
	normal.unitize();

	pt2 = se31.translation - normal * s->radius;
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt2,pt1));
	c->contactModel = cf;
	
	return true;

}

bool BVCFFinalCollider::collideBoxSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{

	if (se31.rotation == Quaternion())
		return collideAABoxSphere(cm1,cm2,se31,se32,c);
	
	Vector3 l,t,p,q,r;
	bool onborder = false;
	Vector3 pt1,pt2,normal;
	Matrix3 axisT,axis;
	float depth;

	shared_ptr<Sphere> s = shared_dynamic_cast<Sphere>(cm2);
	shared_ptr<Box> obb = shared_dynamic_cast<Box>(cm1);
	
	Vector3 extents = obb->extents;

	se31.rotation.toRotationMatrix(axisT);
	axis = axisT.Transpose();
	
	p = se32.translation-se31.translation;
	
	l[0] = extents[0];
	t[0] = axis.GetRow(0).dot(p); 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = extents[1];
	t[1] = axis.GetRow(1).dot(p);
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = extents[2];
	t[2] = axis.GetRow(2).dot(p);
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }
	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		float min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			float tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3 tmp = Vector3(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = axisT*tmp;
		
		normal.unitize();
		
		pt1 = se32.translation + normal*min;
		pt2 = se32.translation - normal*s->radius;	
	
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
		c->contactModel = cf;
		
		return true;	
	}

	q = axisT*t;
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0) 
		return false;

	pt1 = q + se31.translation;

	normal = r;
	normal.unitize();

	pt2 = se32.translation - normal * s->radius;
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
	c->contactModel = cf;
	
	return true;
}

bool BVCFFinalCollider::collideBoxBox(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{

	float r11,r12,r13,r21,r22,r23,r31,r32,r33,q11,q12,q13,q21,q22,q23,q31,q32,q33;
	
	Matrix3 axis1,axis1T,axis2,axis2T;
	Vector3 _normal;
	Vector3 pt1, pt2, o1p1, o2p2;
	Vector3 extents1,extents2;
	
	shared_ptr<Box> obb1 = shared_dynamic_cast<Box>(cm1);
	shared_ptr<Box> obb2 = shared_dynamic_cast<Box>(cm2);

	int nbContacts = 0;
	
	se31.rotation.toRotationMatrix(axis1T);
	axis1 = axis1T.Transpose();
	
	se32.rotation.toRotationMatrix(axis2T);
	axis2 = axis2T.Transpose();
		
	extents1 = obb1->extents;
	extents2 = obb2->extents;
	
	//translation, in parent frame
	Vector3 p = se32.translation-se31.translation;

	//translation of p in A's frame
	Vector3 pp;
	pp = axis1*p;

	//calculate rotation matrix			
	r11 = (axis1.GetRow(0)).dot(axis2.GetRow(0)); r12 = (axis1.GetRow(0)).dot(axis2.GetRow(1)); r13 = (axis1.GetRow(0)).dot(axis2.GetRow(2));
	r21 = (axis1.GetRow(1)).dot(axis2.GetRow(0)); r22 = (axis1.GetRow(1)).dot(axis2.GetRow(1)); r23 = (axis1.GetRow(1)).dot(axis2.GetRow(2));
	r31 = (axis1.GetRow(2)).dot(axis2.GetRow(0)); r32 = (axis1.GetRow(2)).dot(axis2.GetRow(1)); r33 = (axis1.GetRow(2)).dot(axis2.GetRow(2));
	/*Matrix3 R = axis1*axis2T;
	r11 = R[0][0]; r12 = R[0][1]; r13 = R[0][2];
	r21 = R[1][0]; r22 = R[1][1]; r23 = R[1][2];
	r31 = R[2][0]; r32 = R[2][1]; r33 = R[2][2];*/
		
	q11 = fabs(r11); q12 = fabs(r12); q13 = fabs(r13);
	q21 = fabs(r21); q22 = fabs(r22); q23 = fabs(r23);
	q31 = fabs(r31); q32 = fabs(r32); q33 = fabs(r33);

	BoxBoxCollisionInfo bbInfo;
	
	bbInfo.isNormalPrincipalAxis = true;
	bbInfo.invertNormal = false;
	bbInfo.penetrationDepth = -Math::MAX_FLOAT+1;
	bbInfo.code = 0;
	
	// separating axis = u1,u2,u3
	if (!testSeparatingAxis(pp[0],(extents1[0] + extents2[0]*q11 + extents2[1]*q12 + extents2[2]*q13),axis1.GetRow(0),1,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[1],(extents1[1] + extents2[0]*q21 + extents2[1]*q22 + extents2[2]*q23),axis1.GetRow(1),2,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[2],(extents1[2] + extents2[0]*q31 + extents2[1]*q32 + extents2[2]*q33),axis1.GetRow(2),3,&bbInfo))
		return false;

	// separating axis = v1,v2,v3
	if (!testSeparatingAxis(axis2.GetRow(0).dot(p),(extents1[0]*q11+extents1[1]*q21+extents1[2]*q31+extents2[0]),axis2.GetRow(0),4,&bbInfo))
		return false;	

	if (!testSeparatingAxis(axis2.GetRow(1).dot(p),(extents1[0]*q12+extents1[1]*q22+extents1[2]*q32+extents2[1]),axis2.GetRow(1),5,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(axis2.GetRow(2).dot(p),(extents1[0]*q13+extents1[1]*q23+extents1[2]*q33+extents2[2]),axis2.GetRow(2),6,&bbInfo))
		return false;

	// separating axis = u1 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[2]*r21-pp[1]*r31,(extents1[1]*q31+extents1[2]*q21+extents2[1]*q13+extents2[2]*q12),Vector3(0,-r31,r21),7,&bbInfo))
		return false;		
		
	if (!testSeparatingAxis(pp[2]*r22-pp[1]*r32,(extents1[1]*q32+extents1[2]*q22+extents2[0]*q13+extents2[2]*q11),Vector3(0,-r32,r22),8,&bbInfo))
		return false;		
		
	if (!testSeparatingAxis(pp[2]*r23-pp[1]*r33,(extents1[1]*q33+extents1[2]*q23+extents2[0]*q12+extents2[1]*q11),Vector3(0,-r33,r23),9,&bbInfo))
		return false;
		
	// separating axis = u2 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[0]*r31-pp[2]*r11,(extents1[0]*q31+extents1[2]*q11+extents2[1]*q23+extents2[2]*q22),Vector3(r31,0,-r11),10,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[0]*r32-pp[2]*r12,(extents1[0]*q32+extents1[2]*q12+extents2[0]*q23+extents2[2]*q21),Vector3(r32,0,-r12),11,&bbInfo))
		return false;
		
	if (!testSeparatingAxis(pp[0]*r33-pp[2]*r13,(extents1[0]*q33+extents1[2]*q13+extents2[0]*q22+extents2[1]*q21),Vector3(r33,0,-r13),12,&bbInfo))
		return false;	

	// separating axis = u3 x (v1,v2,v3)
	if (!testSeparatingAxis(pp[1]*r11-pp[0]*r21,(extents1[0]*q21+extents1[1]*q11+extents2[1]*q33+extents2[2]*q32),Vector3(-r21,r11,0),13,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[1]*r12-pp[0]*r22,(extents1[0]*q22+extents1[1]*q12+extents2[0]*q33+extents2[2]*q31),Vector3(-r22,r12,0),14,&bbInfo))
		return false;	
		
	if (!testSeparatingAxis(pp[1]*r13-pp[0]*r23,(extents1[0]*q23+extents1[1]*q13+extents2[0]*q32+extents2[1]*q31),Vector3(-r23,r13,0),15,&bbInfo))
		return false;

	int code = bbInfo.code;	
	
	if (code==0) return false;

	bool isNormalPrincipalAxis = bbInfo.isNormalPrincipalAxis;
	bool invertNormal = bbInfo.invertNormal;
	float penetrationDepth = bbInfo.penetrationDepth;
	Vector3 normal = bbInfo.normal;
		
	if (!isNormalPrincipalAxis)	
		_normal = axis1T*normal;
	else	
		_normal = normal;
		
	if (invertNormal)
		normal = -_normal;
	else
		normal = _normal;
		
	normal.unitize(); 
	penetrationDepth = -penetrationDepth;

	if (code > 6) 
	{
		// an edge from box 1 touches an edge from box 2.
		// find a point pa on the intersecting edge of box 1
		Vector3 pa,pb;
		Vector3 sign;

		pa = se31.translation;
		sign[0] = (axis1.GetRow(0).dot(normal) > 0) ? 1.0 : -1.0;
		sign[1] = (axis1.GetRow(1).dot(normal) > 0) ? 1.0 : -1.0;
		sign[2] = (axis1.GetRow(2).dot(normal) > 0) ? 1.0 : -1.0;
		pa+=(sign[0]*extents1[0])*axis1.GetRow(0);
		pa+=(sign[1]*extents1[1])*axis1.GetRow(1);
		pa+=(sign[2]*extents1[2])*axis1.GetRow(2);

		// find a point pb on the intersecting edge of box 2
		pb = se32.translation;
		sign[0] = (axis2.GetRow(0).dot(normal) > 0) ? -1.0 : 1.0;
		sign[1] = (axis2.GetRow(1).dot(normal) > 0) ? -1.0 : 1.0;
		sign[2] = (axis2.GetRow(2).dot(normal) > 0) ? -1.0 : 1.0;
		pb+=(sign[0]*extents2[0])*axis2.GetRow(0);
		pb+=(sign[1]*extents2[1])*axis2.GetRow(1);
		pb+=(sign[2]*extents2[2])*axis2.GetRow(2);

		float alpha,beta;
		Vector3 ua,ub;
		
		ua = axis1[(code-7)/3];
		ub = axis2[(code-7)%3];
		
		lineClosestApproach(pa,ua,pb,ub,alpha,beta);
		pa+=alpha*ua;
		pb+=beta*ub;

		//pt1 = pa;//(pa+pb+normal*penetrationDepth)*0.5;
		//pt2 = pb;//(pa+pb-normal*penetrationDepth)*0.5;
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pa,pb));
		c->contactModel = cf;
		
		nbContacts++;

		return true;	
	}
	
	
	Vector3 pa,pb,Sa,Sb;
	Matrix3 Ra,Rb;
	
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
	Vector3 normal2,nr,anr;
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
	Vector3 center;
	if (nr[lanr] < 0) 
		center = pb - pa + Rb.GetRow(lanr)*Sb[lanr];
	else 
		center = pb - pa - Rb.GetRow(lanr)*Sb[lanr];

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
	std::vector<Vector3> quad;	// 2D coordinate of incident face (x,y pairs)
	float c1,c2,m11,m12,m21,m22;
	c1 = center.dot(Ra[code1]);
	c2 = center.dot(Ra[code2]);
	
	// optimize this? - we have already computed this data above, but it is not
	// stored in an easy-to-index format. for now it's quicker just to recompute
	// the four dot products.
	m11 = Ra.GetRow(code1).dot(Rb.GetRow(a1));
	m12 = Ra.GetRow(code1).dot(Rb.GetRow(a2));
	m21 = Ra.GetRow(code2).dot(Rb.GetRow(a1));
	m22 = Ra.GetRow(code2).dot(Rb.GetRow(a2));
	
	float k1,k2,k3,k4;
	
	k1 = m11*Sb[a1];
	k2 = m21*Sb[a1];
	k3 = m12*Sb[a2];
	k4 = m22*Sb[a2];

	quad.push_back(Vector3(c1 - k1 - k3, c2 - k2 - k4,0));
	quad.push_back(Vector3(c1 - k1 + k3, c2 - k2 + k4,0));
	quad.push_back(Vector3(c1 + k1 + k3, c2 + k2 + k4,0));
	quad.push_back(Vector3(c1 + k1 - k3, c2 + k2 - k4,0));

	// find the size of the reference face
	Vector3 rect;
	rect = Vector3(Sa[code1],Sa[code2],0);

	// intersect the incident and reference faces
	std::vector<Vector3> ret;

	int n = clipPolygon (rect,quad,ret);

	if (n < 1) 
		return false;	// this should never happen

	// convert the intersection points into reference-face coordinates,
	// and compute the contact position and depth for each point. only keep
	// those points that have a positive (penetrating) depth. delete points in
	// the 'ret' array as necessary so that 'point' and 'ret' correspond.
	std::vector<Vector3> point;	// penetrating contact points
	std::vector<float> dep;			// depths for those points

	float det1 = 1.0/(m11*m22 - m12*m21);

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
		
		point[cnum] = center + k1*Rb.GetRow(a1) + k2*Rb.GetRow(a2);
		
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
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
		}
		else
		{
			//pt2		= point[j] + pa;
			//pt1 	= pt2-normal*dep[j];
			pt1		= point[j] + pa;
			pt2 	= pt1-normal*dep[j];
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt1,pt2));
		}
		nbContacts++;
	}
	c->contactModel = cf;
	
	return true;
}

void BVCFFinalCollider::lineClosestApproach (const Vector3 pa, const Vector3 ua, const Vector3 pb, const Vector3 ub, float &alpha, float &beta)
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

bool BVCFFinalCollider::testSeparatingAxis(float expr1, float expr2, Vector3 n,int c,BoxBoxCollisionInfo* bbInfo) 
{
	float pd,l;

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

int BVCFFinalCollider::clipPolygon(Vector3 quad,const std::vector<Vector3>& polygon, std::vector<Vector3>& clipped)
{

	unsigned int v;
	int d;
	std::vector<Vector3> tmpPoly;
	
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

void BVCFFinalCollider::clipLeft(float sizeX, std::vector<Vector3> &polygon, Vector3 v1, Vector3 v2)
{

	float dx,dy,m;
	Vector3 v;
	
   
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

void BVCFFinalCollider::clipRight(float sizeX, std::vector<Vector3>& polygon, Vector3 v1, Vector3 v2)
{
	
	float dx,dy,m;
	Vector3 v;
	
   
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

void BVCFFinalCollider::clipTop(float sizeY, std::vector<Vector3>& polygon, Vector3 v1, Vector3 v2)
{
	
	float dx,dy,m;
	Vector3 v;
	
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

void BVCFFinalCollider::clipBottom(float sizeY, std::vector<Vector3> &polygon, Vector3 v1, Vector3 v2)
{
	
	float dx,dy,m;
	Vector3 v;
	   
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


bool BVCFFinalCollider::collideTerrainSphere(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
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
//	else
//		delete cf;
	
	return inCollision;
}

bool BVCFFinalCollider::collideSphereTerrain(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	Vector3 pt;
	std::vector<Vector3> tri;
	bool inCollision =  false;
	std::vector<int> faces;
	
	shared_ptr<Sphere> s  = shared_dynamic_cast<Sphere>(cm1);
	shared_ptr<Terrain> t = shared_dynamic_cast<Terrain>(cm2);
		
	Vector3 min,max;
	Vector3 radius = Vector3(s->radius,s->radius,s->radius);
	min = se31.translation-radius;
	max = se31.translation+radius;
	
	t->getFaces(AABB((max-min)*0.5,(min+max)*0.5),faces);
	tri.resize(3);

	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	
	for(unsigned int i=0;i<faces.size();i++)
	{
		int faceId = faces[i];
		tri[0] = t->vertices[t->faces[faceId][0]]+se32.translation;
		tri[1] = t->vertices[t->faces[faceId][1]]+se32.translation;
		tri[2] = t->vertices[t->faces[faceId][2]]+se32.translation;
		float d = sqrDistTriPoint(se31.translation, tri, pt);

		if (d<s->radius*s->radius)
		{
			Vector3 v = se31.translation-pt;
			v.unitize();
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(se31.translation-v*s->radius,pt));
			inCollision =  true;
		}
	}
	
	if (inCollision)
		c->contactModel = cf;
//	else
//		delete cf;
	
	return inCollision;
}

float BVCFFinalCollider::sqrDistTriPoint(const Vector3& p, const std::vector<Vector3>& tri, Vector3& pt)
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

	//if ( pfSParam )
	//	*pfSParam = fS;

	//if ( pfTParam )
	//	*pfTParam = fT;

	pt = tri[0]+fS*edge0+fT*edge1;
		
	return fabs(fSqrDist);
}



bool BVCFFinalCollider::collideTerrainBox(const shared_ptr<CollisionModel> , const shared_ptr<CollisionModel> ,const Se3& , const Se3& , shared_ptr<Contact> )
{
	return false;
}

bool BVCFFinalCollider::collideBoxTerrain(const shared_ptr<CollisionModel>, const shared_ptr<CollisionModel>,const Se3& , const Se3& , shared_ptr<Contact>)
{
/*
template <class Real>
bool Wml::FindIntersection (const Triangle3<Real>& rkTri,
    const Box3<Real>& rkBox, int& riQuantity, Vector3<Real> akP[6])
{
    riQuantity = 3;
    akP[0] = rkTri.Origin();
    akP[1] = rkTri.Origin() + rkTri.Edge0();
    akP[2] = rkTri.Origin() + rkTri.Edge1();

    for (int iDir = -1; iDir <= 1; iDir += 2)
    {
        for (int iSide = 0; iSide < 3; iSide++)
        {
            Vector3<Real> kInnerNormal = ((Real)iDir)*rkBox.Axis(iSide);
            Real fConstant = kInnerNormal.Dot(rkBox.Center()) -
                rkBox.Extent(iSide);
            ClipConvexPolygonAgainstPlane(kInnerNormal,fConstant,riQuantity,
                akP);
        }
    }

    return riQuantity > 0;
}*/
	return false;
}

/*static void ProjectTriangle (const Vector3<Real>& rkD,
    const Vector3<Real> akV[3], Real& rfMin, Real& rfMax)
{
    rfMin = rkD.Dot(akV[0]);
    rfMax = rfMin;

    Real fDot = rkD.Dot(akV[1]);
    if ( fDot < rfMin )
        rfMin = fDot;
    else if ( fDot > rfMax )
        rfMax = fDot;

    fDot = rkD.Dot(akV[2]);
    if ( fDot < rfMin )
        rfMin = fDot;
    else if ( fDot > rfMax )
        rfMax = fDot;
}
//----------------------------------------------------------------------------
template <class Real>
static void ProjectBox (const Vector3<Real>& rkD, const Box3<Real>& rkBox,
    Real& rfMin, Real& rfMax)
{
    Real fDdC = rkD.Dot(rkBox.Center());
    Real fR =
        rkBox.Extent(0)*Math<Real>::FAbs(rkD.Dot(rkBox.Axis(0))) +
        rkBox.Extent(1)*Math<Real>::FAbs(rkD.Dot(rkBox.Axis(1))) +
        rkBox.Extent(2)*Math<Real>::FAbs(rkD.Dot(rkBox.Axis(2)));
    rfMin = fDdC - fR;
    rfMax = fDdC + fR;
}
//----------------------------------------------------------------------------
template <class Real>
static bool NoIntersect (Real fTMax, Real fSpeed, Real fMin0, Real fMax0,
    Real fMin1, Real fMax1, Real& rfTFirst, Real& rfTLast)
{
    Real fInvSpeed, fT;

    if ( fMax1 < fMin0 )  // C1 initially on left of C0
    {
        if ( fSpeed <= (Real)0.0 )
        {
            // intervals moving apart
            return true;
        }

        fInvSpeed = ((Real)1.0)/fSpeed;

        fT = (fMin0 - fMax1)*fInvSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;
        if ( rfTFirst > fTMax )
            return true;

        fT = (fMax0 - fMin1)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;
        if ( rfTFirst > rfTLast )
            return true;
    }
    else if ( fMax0 < fMin1 )  // C1 initially on right of C0
    {
        if ( fSpeed >= (Real)0.0 )
        {
            // intervals moving apart
            return true;
        }

        fInvSpeed = ((Real)1.0)/fSpeed;

        fT = (fMax0 - fMin1)*fInvSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;
        if ( rfTFirst > fTMax )
            return true;

        fT = (fMin0 - fMax1)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;
        if ( rfTFirst > rfTLast )
            return true;
    }
    else  // C0 and C1 overlap
    {
        if ( fSpeed > (Real)0.0 )
        {
            fT = (fMax0 - fMin1)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;
            if ( rfTFirst > rfTLast )
                return true;
        }
        else if ( fSpeed < (Real)0.0 )
        {
            fT = (fMin0 - fMax1)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;
            if ( rfTFirst > rfTLast )
                return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Wml::TestIntersection (const Triangle3<Real>& rkTri,
    const Box3<Real>& rkBox)
{
    // get triangle vertices
    Vector3<Real> akV[3] =
    {
        rkTri.Origin(),
        rkTri.Origin() + rkTri.Edge0(),
        rkTri.Origin() + rkTri.Edge1()
    };

    Real fMin0, fMax0, fMin1, fMax1;
    Vector3<Real> kD, akE[3];

    // test direction of triangle normal
    akE[0] = akV[1] - akV[0];
    akE[1] = akV[2] - akV[0];
    kD = akE[0].Cross(akE[1]);
    fMin0 = kD.Dot(akV[0]);
    fMax0 = fMin0;
    ProjectBox(kD,rkBox,fMin1,fMax1);
    if ( fMax1 < fMin0 || fMax0 < fMin1 )
        return false;

    // test direction of box faces
    for (int i = 0; i < 3; i++)
    {
        kD = rkBox.Axis(i);
        ProjectTriangle(kD,akV,fMin0,fMax0);
        Real fDdC = kD.Dot(rkBox.Center());
        fMin1 = fDdC - rkBox.Extent(i);
        fMax1 = fDdC + rkBox.Extent(i);
        if ( fMax1 < fMin0 || fMax0 < fMin1 )
            return false;
    }

    // test direction of triangle-box edge cross products
    akE[2] = akE[1] - akE[0];
    for (int i0 = 0; i0 < 3; i0++)
    {
        for (int i1 = 0; i1 < 3; i1++)
        {
            kD = akE[i0].Cross(rkBox.Axis(i1));
            ProjectTriangle(kD,akV,fMin0,fMax0);
            ProjectBox(kD,rkBox,fMin1,fMax1);
            if ( fMax1 < fMin0 || fMax0 < fMin1 )
                return false;
        }
    }

    return true;
}*/
