#include "MultiMethodsManager.hpp"
#include "Indexable.hpp"
#include "CollisionModel.hpp"
#include "CollisionFunctor.hpp"

inline bool MultiMethodsManager::operator() (const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
{
	assert(cm1->getClassIndex()>0);
	assert(cm2->getClassIndex()>0);
	assert((unsigned int)(cm1->getClassIndex())<callBacks.size());
	assert((unsigned int)(cm2->getClassIndex())<callBacks.size());
	shared_ptr<CollisionFunctor> cf = callBacks[cm1->getClassIndex()][cm2->getClassIndex()];
	if (cf!=0)
		return (*cf)(cm1,cm2,se31,se32,c);
	else
		return false;
}



bool MultiMethodsManager::add(const string& name)
{
	if (indexedClassName.find(name)!=indexedClassName.end())
	{
		shared_ptr<Indexable> indexable = shared_dynamic_cast<Indexable>(ClassFactory::instance().createShared(name));
		int& index = indexable->getClassIndex();
		assert(index==-1);
		index = indexedClassName.size()-1;
		indexedClassName[name] = index;
		map<string,int>::iterator icni    = indexedClassName.begin();
		map<string,int>::iterator icniEnd = indexedClassName.end();
		for( ;icni!=icniEnd; ++icni )
		{
			string functorName = name+"2"+(*icni).first+"4ClosestFeatures";
			string reverseFunctorName = (*icni).first+"2"+name+"4ClosestFeatures";
			shared_ptr<CollisionFunctor> collisionFunctor,reverseCollisionFunctor;
			try
			{
				collisionFunctor = shared_dynamic_cast<CollisionFunctor>(ClassFactory::instance().createShared(functorName));
				reverseCollisionFunctor = shared_dynamic_cast<CollisionFunctor>(ClassFactory::instance().createShared(functorName));
				collisionFunctor->setReverse(false);
				reverseCollisionFunctor->setReverse(true);
			}
			catch (FactoryError& fe)
			{
				collisionFunctor = shared_dynamic_cast<CollisionFunctor>(ClassFactory::instance().createShared(reverseFunctorName));
				reverseCollisionFunctor = shared_dynamic_cast<CollisionFunctor>(ClassFactory::instance().createShared(reverseFunctorName));
				collisionFunctor->setReverse(true);
				reverseCollisionFunctor->setReverse(false);
			}

			callBacks[indexedClassName[(*icni).first]][index] = reverseCollisionFunctor;
			callBacks[index][indexedClassName[(*icni).first]] = collisionFunctor;
		}
		return true;
	}
	else
		return false;
}
