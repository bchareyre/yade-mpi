#include "MultiMethodsManager.hpp"
#include "Indexable.hpp"
#include "CollisionModel.hpp"
#include "CollisionFunctor.hpp"


MultiMethodsManager::MultiMethodsManager ()
{
	callBacks.resize(0);
	indexedClassName.resize(0);
}

MultiMethodsManager::~MultiMethodsManager ()
{
}

//bool MultiMethodsManager::addInteractionModel(const string& name)
//{
//
//}

bool MultiMethodsManager::addCollisionGeometry(const string& name)
{
	shared_ptr<CollisionModel> cm  = dynamic_pointer_cast<CollisionModel>(ClassFactory::instance().createShared(name));
	int& index = cm->getClassIndex();

	if (index==-1)
	{
		index = indexedClassName.size();
		indexedClassName.push_back(name);

		// resizing callBacks table
		callBacks.resize( indexedClassName.size() );
		std::vector<std::vector<shared_ptr<CollisionFunctor> > >::iterator ci    = callBacks.begin();
		std::vector<std::vector<shared_ptr<CollisionFunctor> > >::iterator ciEnd = callBacks.end();
		for( ; ci != ciEnd ; ++ci )
			(*ci).resize( indexedClassName.size() );

		bool found=false;
		for(unsigned int i=0 ;i<indexedClassName.size();i++)
		{
			string functorName = name+"2"+indexedClassName[i]+"4ClosestFeatures";
			string reverseFunctorName = indexedClassName[i]+"2"+name+"4ClosestFeatures";
			shared_ptr<CollisionFunctor> collisionFunctor,reverseCollisionFunctor;
			try
			{
				collisionFunctor = dynamic_pointer_cast<CollisionFunctor>(ClassFactory::instance().createShared(functorName));
				reverseCollisionFunctor = dynamic_pointer_cast<CollisionFunctor>(ClassFactory::instance().createShared(functorName));
				collisionFunctor->setReverse(false);
				reverseCollisionFunctor->setReverse(true);
				found = true;
			}
			catch (FactoryCantCreate& fe)
			{
				try
				{
					collisionFunctor = dynamic_pointer_cast<CollisionFunctor>(ClassFactory::instance().createShared(reverseFunctorName));
					reverseCollisionFunctor = dynamic_pointer_cast<CollisionFunctor>(ClassFactory::instance().createShared(reverseFunctorName));
					collisionFunctor->setReverse(true);
					reverseCollisionFunctor->setReverse(false);
					found = true;
				}
				catch (FactoryCantCreate& fe)
				{
					found = false;
				}
			}

 			if (found)
			{
				callBacks[i][index] = reverseCollisionFunctor;
				callBacks[index][i] = collisionFunctor;
			}
			found=false;
		}

		#ifdef DEBUG
			cerr <<" New class added to NarrowColliderMultiMethodsManager: " << name << endl;
		#endif

		return true;
	}
	else
		return false;
}

//bool MultiMethodsManager::extendCallBacksTable()
//{
///}

bool MultiMethodsManager::go(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c)
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
