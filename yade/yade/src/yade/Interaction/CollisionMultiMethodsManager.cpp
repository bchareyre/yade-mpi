#include "CollisionMultiMethodsManager.hpp"
#include "Indexable.hpp"
#include "CollisionGeometry.hpp"
#include "CollisionFunctor.hpp"


CollisionMultiMethodsManager::CollisionMultiMethodsManager () : MultiMethodsManager<CollisionFunctor>()
{
}

CollisionMultiMethodsManager::~CollisionMultiMethodsManager ()
{
}

bool CollisionMultiMethodsManager::addPair(const string& name1,const string& name2,const string& libName)
{
	shared_ptr<CollisionGeometry> cm1  = dynamic_pointer_cast<CollisionGeometry>(ClassFactory::instance().createShared(name1));
	shared_ptr<CollisionGeometry> cm2  = dynamic_pointer_cast<CollisionGeometry>(ClassFactory::instance().createShared(name2));
	shared_ptr<Indexable> i1 = dynamic_pointer_cast<Indexable>(cm1);
	shared_ptr<Indexable> i2 = dynamic_pointer_cast<Indexable>(cm2);

	return add(i1, i2, name1, name2, libName);
}

bool CollisionMultiMethodsManager::collide(const shared_ptr<CollisionGeometry> cm1, const shared_ptr<CollisionGeometry> cm2, const Se3r& se31, const Se3r& se32, shared_ptr<Interaction> c)
{
	assert(cm1->getClassIndex()>=0);
	assert(cm2->getClassIndex()>=0);
	assert((unsigned int)(cm1->getClassIndex())<callBacks.size());
	assert((unsigned int)(cm2->getClassIndex())<callBacks.size());

	shared_ptr<CollisionFunctor> cf = callBacks[cm1->getClassIndex()][cm2->getClassIndex()];

	if (cf!=0)
		return (*cf)(cm1,cm2,se31,se32,c);
	else
		return false;
}
