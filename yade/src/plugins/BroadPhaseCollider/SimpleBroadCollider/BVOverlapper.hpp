#ifndef __BVOVERLAPPER_H__
#define __BVOVERLAPPER_H__

#include "Overlapper.hpp"
#include "BoundingVolume.hpp"

class BVOverlapper : public Overlapper
{
	
	public : BVOverlapper ();
	public : ~BVOverlapper ();
		
	public : void registerAttributes();
	public : void processAttributes();
	
	protected : static bool overlappSphereSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static inline bool overlappSphereAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
	{
		return overlappAABBSphere(bv2,bv1);
	}
	protected : static bool overlappAABBSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static bool overlappAABBAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);

	REGISTER_CLASS_NAME(BVOverlapper);

};

REGISTER_CLASS(BVOverlapper,false);

#endif // __BVOVERLAPPER_H__
