/*CWBoon 2015 */

#pragma once
#ifdef YADE_POTENTIAL_PARTICLES

#include <pkg/common/Dispatching.hpp>
#include <pkg/dem/PotentialParticle.hpp>

class PotentialParticle2AABB : public BoundFunctor {
	public :

		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);

		FUNCTOR1D(PotentialParticle);
		//REGISTER_ATTRIBUTES(BoundFunctor,(aabbEnlargeFactor));
		YADE_CLASS_BASE_DOC_ATTRS(PotentialParticle2AABB,BoundFunctor,"EXPERIMENTAL. Functor creating :yref:`Aabb` from :yref:`PotentialParticle`.",
			((Real,aabbEnlargeFactor,((void)"deactivated",-1),,"see :yref:`Sphere2AABB`."))
			((Vector3r, halfSize, Vector3r::Zero(),,"halfSize"))

		);
};

REGISTER_SERIALIZABLE(PotentialParticle2AABB);

#endif // YADE_POTENTIAL_PARTICLES
