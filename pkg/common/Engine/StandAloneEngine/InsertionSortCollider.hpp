// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
class InteractionContainer;
/* Collider that should run in O(n log(n)) time, but faster than PersistentSAPCollider.

	At the initial step, Bodies' bounds (along sortAxis) are first std::sort'ed along one axis (sortAxis), then collided.

	Insertion sort is used for sorting the bound list that is already pre-sorted from last iteration, where each inversion
	calls checkOverlap which then handles either overlap (by creating interaction if necessary) or its absence (by deleting
	interaction if it is only potential).

	Bodies without bounding volume are handled gracefully and never collide.
*/

#define COLLIDE_STRIDED

// #define this macro to enable timing whichin this engine
//#define ISC_TIMING

#ifdef ISC_TIMING
	#define ISC_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
#else
	#define ISC_CHECKPOINT(cpt)
#endif

class BoundingVolumeMetaEngine;
class NewtonsDampedLaw;

class InsertionSortCollider: public Collider{
	//! struct for storing bounds of bodies
	struct Bound{
		//! coordinate along the given sortAxis
		Real coord;
		//! id of the body this bound belongs to
		body_id_t id;
		//! is it the minimum (true) or maximum (false) bound?
		struct{ unsigned hasBB:1; unsigned isMin:1; } flags;
		Bound(Real coord_, body_id_t id_, bool isMin): coord(coord_), id(id_){ flags.isMin=isMin; }
		bool operator<(const Bound& b) const {
			/* handle special case of zero-width bodies, which could otherwise get min/max swapped in the unstable std::sort */
			if(id==b.id && coord==b.coord) return flags.isMin;
			return coord<b.coord;
		}
		bool operator>(const Bound& b) const {
			if(id==b.id && coord==b.coord) return b.flags.isMin;
			return coord>b.coord;
		}
	};
	#ifdef COLLIDE_STRIDED
		// keep this dispatcher and call it ourselves as needed
		shared_ptr<BoundingVolumeMetaEngine> boundDispatcher;
		// we need this to find out about current maxVelocitySq
		shared_ptr<NewtonsDampedLaw> newton;
		// if False, no type of striding is used
		// if True, then either sweepLength XOR nBins is set
		bool strideActive;
		public:
		/// Absolute length that will be added to bounding boxes at each side; it should be something like 1/5 of typical grain radius
		/// this value is used to adapt stride; if too large, stride will be big, but the ratio of potential-only interactions will be very big, 
		/// thus slowing down collider & interaction loops significantly (remember: O(addLength^3))
		/// If non-positive, collider runs always, without stride adaptivity
		Real sweepLength;
		//! Overestimation factor for the sweep velocity; must be >=1.0.
		/// Has no influence on sweepLength, only on the computed stride.
		/// Default 1.05
		Real sweepFactor;
		//! maximum distance that the fastest body could have travelled since the last run; if >= sweepLength, we could get out of bboxes and will trigger full run
		Real fastestBodyMaxDist;
		// parameters to be passed to VelocityBins, if nBins>0
		int nBins; Real binCoeff, binOverlap;
	#endif
	private:
	//! storage for bounds
	std::vector<Bound> XX,YY,ZZ;
	//! storage for bb maxima and minima
	std::vector<Real> maxima, minima;



	/*! sorting routine; insertion sort is very fast for strongly pre-sorted lists, which is our case
  	    http://en.wikipedia.org/wiki/Insertion_sort has the algorithm and other details
	*/
	void insertionSort(std::vector<Bound>& v,InteractionContainer*,MetaBody*,bool doCollide=true);
	void handleBoundInversion(body_id_t,body_id_t,InteractionContainer*,MetaBody*);
	bool spatialOverlap(body_id_t,body_id_t) const;

	public:
	//! axis for the initial sort
	int sortAxis;
	//! if true, separate sorting and colliding phase; MUCH slower, but processes all interactions at every step
	// This makes the collider non-persistent, not remembering last state
	bool sortThenCollide;
	//! Predicate called from loop within InteractionContainer::erasePending
	bool shouldBeErased(body_id_t id1, body_id_t id2, MetaBody*) const { return !spatialOverlap(id1,id2); }
	#ifdef COLLIDE_STRIDED
		virtual bool isActivated(MetaBody*);
	#endif

	bool probeBoundingVolume(const BoundingVolume&);

	InsertionSortCollider():
	#ifdef COLLIDE_STRIDED
		strideActive(false), sweepLength(-1), sweepFactor(1.05), fastestBodyMaxDist(-1), nBins(0), binCoeff(5), binOverlap(0.8),
	#endif
		sortAxis(0), sortThenCollide(false){
			#ifdef ISC_TIMING
				timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
			#endif 
		 }
	virtual void action(MetaBody*);
	REGISTER_CLASS_AND_BASE(InsertionSortCollider,Collider);
	REGISTER_ATTRIBUTES(Collider,(sortAxis)(sortThenCollide)
		#ifdef COLLIDE_STRIDED
			(strideActive)(sweepLength)(sweepFactor)(fastestBodyMaxDist)(nBins)(binCoeff)(binOverlap)
		#endif
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InsertionSortCollider);
