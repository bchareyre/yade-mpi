#include "BoundingVolumeFactoryManager.hpp"
#include "Indexable.hpp"
#include "CollisionGeometry.hpp"
#include "BoundingVolumeFactory.hpp"
#include "BoundingVolume.hpp"

BoundingVolumeFactoryManager::BoundingVolumeFactoryManager () : MultiMethodsManager<BoundingVolumeFactory>()
{
}

BoundingVolumeFactoryManager::~BoundingVolumeFactoryManager ()
{
}

bool BoundingVolumeFactoryManager::addPair(const string& name1,const string& name2,const string& libName)
{
	shared_ptr<CollisionGeometry> cm	= dynamic_pointer_cast<CollisionGeometry>(ClassFactory::instance().createShared(name1));
	shared_ptr<BoundingVolume> bv		= dynamic_pointer_cast<BoundingVolume>(ClassFactory::instance().createShared(name2));
	shared_ptr<Indexable> i1 = dynamic_pointer_cast<Indexable>(cm);
	shared_ptr<Indexable> i2 = dynamic_pointer_cast<Indexable>(bv);

	return add(i1, i2, name1, name2, libName);
}

void BoundingVolumeFactoryManager::buildBoundingVolume(const shared_ptr<CollisionGeometry> cm, const Se3r& se3,shared_ptr<BoundingVolume> bv)
{
	assert(cm->getClassIndex()>=0);
	assert(bv->getClassIndex()>=0);
	assert((unsigned int)(cm->getClassIndex())<callBacks.size());
	assert((unsigned int)(bv->getClassIndex())<callBacks.size());

	shared_ptr<BoundingVolumeFactory> bvf = callBacks[cm->getClassIndex()][bv->getClassIndex()];

	if (bvf!=0)
		(*bvf)(cm,se3,bv);
}
