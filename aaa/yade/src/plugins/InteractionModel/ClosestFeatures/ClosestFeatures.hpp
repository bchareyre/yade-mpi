#ifndef __CLOSESTSFEATURES_H__
#define __CLOSESTSFEATURES_H__

#include <vector>

#include "ContactModel.hpp"
#include "Vector3.hpp"

// FIXME : ContactModel   into   InteractionModel
class ClosestFeatures : public ContactModel
{	
	public : std::vector<std::pair<Vector3,Vector3> > closestsPoints;

	// construction
	public : ClosestFeatures ();	
	public : ~ClosestFeatures ();	
	
	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(ClosestFeatures);
};

REGISTER_CLASS(ClosestFeatures,false);

#endif // __CLOSESTSFEATURES_H__
