#include "SimpleBroadCollider.hpp"
//#include "BVOverlapper.hpp"
#include "Body.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SimpleBroadCollider::SimpleBroadCollider () : BroadPhaseCollider()
{
	nbPotentialCollisions = 0;
	//overlapper = shared_ptr<Overlapper>(new BVOverlapper());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SimpleBroadCollider::~SimpleBroadCollider ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimpleBroadCollider::processAttributes()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimpleBroadCollider::registerAttributes()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int SimpleBroadCollider::broadPhaseCollisionTest(const std::vector<shared_ptr<Body> >& bodies,std::list<shared_ptr<Interaction> >& interactions)
{	
	nbPotentialCollisions=0;
	
	interactions.clear();
	
	/*for(unsigned int i=0;i<bodies.size()-1 ; i++)
	{
		shared_ptr<BoundingVolume> bv1 = bodies[i]->bv;
		for(unsigned int j=i+1;j<bodies.size() ; j++)
		{	
			shared_ptr<BoundingVolume> bv2 = bodies[j]->bv;
			if ( !(bodies[i]->isDynamic==false && bodies[j]->isDynamic==false) && overlapper->overlapp(bv1,bv2))
			{
				interactions.push_back(shared_ptr<Interaction>(new Interaction(i,j)));
				nbPotentialCollisions++;
			}
		}
	}*/
	  
	return nbPotentialCollisions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


/*

bool BVOverlapper::overlappSphereSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
{
	shared_ptr<BoundingSphere> s1 = dynamic_pointer_cast<BoundingSphere>(bv1);
	shared_ptr<BoundingSphere> s2 = dynamic_pointer_cast<BoundingSphere>(bv2);
	
	return ((s1->center-s2->center).length()<s1->radius+s2->radius);
	
}

bool BVOverlapper::overlappAABBSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
{

	Vector3 l,t,p,q,r;
	bool onborder = false;
	Vector3 pt1,pt2,normal;
	float depth;
	
	shared_ptr<BoundingSphere> s = dynamic_pointer_cast<BoundingSphere>(bv2);
	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv1);
	
	p = s->center-aabb->center;
	
	l[0] = aabb->halfSize[0];
	t[0] = p[0]; 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = aabb->halfSize[1];
	t[1] = p[1];
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = aabb->halfSize[2];
	t[2] = p[2];
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }

	if (!onborder) 
		return true;

	q = t;
	
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	return (depth >= 0);
	
}


bool BVOverlapper::overlappAABBAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
{
	Vector3 min1 = bv1->min;
	Vector3 max1 = bv1->max;
	Vector3 min2 = bv2->min;
	Vector3 max2 = bv2->max;
	
	return !(max1[0]<min2[0] || max2[0]<min1[0] || max1[1]<min2[1] || max2[1]<min1[1] || max1[2]<min2[2] || max2[2]<min1[2]);
}

*/
