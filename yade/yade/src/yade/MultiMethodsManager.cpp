#include "MultiMethodsManager.hpp"
#include "Indexable.hpp"
#include "CollisionModel.hpp"
#include "CollisionFunctor.hpp"

bool MultiMethodsManager::go(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
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
		cerr << "maybe Adding " << name << endl;
	if (indexedClassName.find(name)==indexedClassName.end())
	{
		//indexedClassName[name] = 0;
cerr <<1<<endl;
		//shared_ptr<Indexable> indexable  = shared_dynamic_cast<Indexable>(ClassFactory::instance().createShared(name));
		shared_ptr<Serializable> s  = ClassFactory::instance().createShared(name);
		shared_ptr<Indexable> indexable = shared_dynamic_cast<Indexable>(s);
		if (ClassFactory::instance().createShared(name) == 0)
			cerr <<"OOOOOOOOOOOOOOOops!\n";
		
cerr <<2<<endl;
		int& index = indexable->getClassIndex();
		assert(index==-1);
cerr <<3<<endl;
		index = indexedClassName.size()-1;
		indexedClassName[name] = index;
cerr <<4<<endl;
		map<string,int>::iterator icni    = indexedClassName.begin();
		map<string,int>::iterator icniEnd = indexedClassName.end();
		for( ;icni!=icniEnd; ++icni )
		{
cerr <<5<<endl;
			string functorName = name+"2"+(*icni).first+"4ClosestFeatures";
			string reverseFunctorName = (*icni).first+"2"+name+"4ClosestFeatures";
cerr <<6<<endl;
			shared_ptr<CollisionFunctor> collisionFunctor,reverseCollisionFunctor;
cerr <<7<<endl;
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

		cerr << "Adding " << name << endl;
		
		return true;
	}
	else
		return false;
}
