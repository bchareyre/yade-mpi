#include "Overlapper.hpp"

#include <iostream>
using namespace std;

Overlapper::Overlapper () : Serializable(), MultiMethodsManager<pt2OverlappingFunc>()
{
	nullFunc = &nullOverlappingFunc;
}

Overlapper::~Overlapper ()
{

}
	
bool Overlapper::nullOverlappingFunc(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
{

	cerr << "*** RUNTIME ERROR : NO FUNCTION DEFINE FOR BOUNDING VOLUME OF TYPE " << bv1->type << " AND BOUNDING VOLUME OF TYPE " << bv2->type << " ***" << endl;
	return false;
}
