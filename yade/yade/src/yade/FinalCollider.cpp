#include "FinalCollider.hpp"

#include <iostream>
using namespace std;

FinalCollider::FinalCollider() : Serializable(), MultiMethodsManager<ptr2CollidingFunc>()
{
	nullFunc = nullCollideFunc;
}

FinalCollider::~FinalCollider ()
{

}


bool FinalCollider::nullCollideFunc(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& , const Se3& , shared_ptr<Contact> )
{

	cerr << "*** RUNTIME ERROR : NO FUNCTION DEFINE FOR COLLIDING COLLISION MODEL OF TYPE " << cm1->type << " AND COLLISION MODEL OF TYPE " << cm2->type << " ***" << endl;
	return false;
}
