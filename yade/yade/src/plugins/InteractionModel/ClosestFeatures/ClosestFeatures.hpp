#ifndef __CLOSESTSFEATURES_H__
#define __CLOSESTSFEATURES_H__

#include <vector>

#include "InteractionModel.hpp"
#include "Vector3.hpp"

// FIXME : InteractionModel   into   InteractionModel
class ClosestFeatures : public InteractionModel
{
	public : std::vector<std::pair<Vector3,Vector3> > closestsPoints;
	public : std::vector<int> verticesId;
	// construction
	public : ClosestFeatures ();
	public : ~ClosestFeatures ();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(ClosestFeatures);
	//REGISTER_CLASS_INDEX(ClosestFeatures);
};

REGISTER_SERIALIZABLE(ClosestFeatures,false);

#endif // __CLOSESTSFEATURES_H__
