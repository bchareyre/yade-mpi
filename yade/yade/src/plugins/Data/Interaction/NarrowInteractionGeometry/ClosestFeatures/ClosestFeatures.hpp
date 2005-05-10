#ifndef __CLOSESTSFEATURES_H__
#define __CLOSESTSFEATURES_H__

#include <vector>

#include <yade/InteractionGeometry.hpp>
#include <yade-lib-wm3-math/Vector3.hpp>

class ClosestFeatures : public InteractionGeometry
{
	public : ClosestFeatures();
	public : virtual ~ClosestFeatures();
	
	public : std::vector<std::pair<Vector3r,Vector3r> > closestsPoints;
	public : std::vector<int> verticesId;

	REGISTER_CLASS_NAME(ClosestFeatures);
};

REGISTER_SERIALIZABLE(ClosestFeatures,false);

#endif // __CLOSESTSFEATURES_H__

