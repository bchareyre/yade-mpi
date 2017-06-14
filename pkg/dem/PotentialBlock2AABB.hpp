/*CWBoon 2015 */
#ifdef YADE_POTENTIAL_BLOCKS
#pragma once

#include<pkg/common/Dispatching.hpp>
#include<pkg/dem/PotentialBlock.hpp>

class PotentialBlock2AABB : public BoundFunctor
{
	public :
		
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
		
	FUNCTOR1D(PotentialBlock);
	//REGISTER_ATTRIBUTES(BoundFunctor,(aabbEnlargeFactor));
	YADE_CLASS_BASE_DOC_ATTRS(PotentialBlock2AABB,BoundFunctor,"Functor creating :yref:`Aabb` from :yref:`Sphere`.",
		((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"Relative enlargement of the bounding box; deactivated if negative.\n\n.. note::\n\tThis attribute is used to create distant interaction, but is only meaningful with an :yref:`InteractionGeometryFunctor` which will not simply discard such interactions: :yref:`Ig2_Sphere_Sphere_Dem3DofGeom::distFactor` / :yref:`Ig2_Sphere_Sphere_ScGeom::interactionDetectionFactor` should have the same value as :yref:`aabbEnlargeFactor<Bo1_Sphere_Aabb::aabbEnlargeFactor>`."))
		((Vector3r, halfSize, Vector3r::Zero(),,"halfSize"))
		
	);
};

REGISTER_SERIALIZABLE(PotentialBlock2AABB);
#endif // YADE_POTENTIAL_BLOCKS
