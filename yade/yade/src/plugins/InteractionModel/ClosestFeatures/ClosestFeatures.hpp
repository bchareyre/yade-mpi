#ifndef __CLOSESTSFEATURES_H__
#define __CLOSESTSFEATURES_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"

class ClosestFeatures : public InteractionGeometry
{
	public : std::vector<std::pair<Vector3,Vector3> > closestsPoints;
	public : std::vector<int> verticesId;
	// construction
	public : ClosestFeatures ();
	public : ~ClosestFeatures ();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(ClosestFeatures);
};

REGISTER_SERIALIZABLE(ClosestFeatures,false);

#endif // __CLOSESTSFEATURES_H__
