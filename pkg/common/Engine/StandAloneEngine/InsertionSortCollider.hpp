// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
#include<yade/core/World.hpp>
class InteractionContainer;
/* Collider that should run in O(n log(n)) time, but faster than historical PersistentSAPCollider.

	At the initial step, Bodies' bounds (along sortAxis) are first std::sort'ed along one axis (sortAxis), then collided.

	Insertion sort is used for sorting the bound list that is already pre-sorted from last iteration, where each inversion
	calls checkOverlap which then handles either overlap (by creating interaction if necessary) or its absence (by deleting
	interaction if it is only potential).

	Bodies without bounding volume are handled gracefully and never collide.
*/


/*! Periodic collider notes.

Use
===
* scripts/test/periodic-simple.py
* In the future, triaxial compression working by growing/shrinking the cell should be implemented.

Architecture
============
Values from bounding boxes are added information about period in which they are.
Their container (VecBounds) holds position of where the space wraps.
The sorting algorithm is changed in such way that periods are changed when body crosses cell boundary.

Interaction::cellDist holds information about relative cell coordinates of the 2nd body
relative to the 1st one. Dispatchers (InteractionGeometryDispatcher and InteractionDispatchers)
use this information to pass modified position of the 2nd body to InteractionGeometryFunctors.
Since properly behaving InteractionGeometryFunctor's and ConstitutiveLaw's do not take positions
directly from Body::state, the interaction is computed with the periodic positions.

Positions of bodies (in the sense of Body::state) and their natural bboxes are not wrapped
to the periodic cell, they can be anywhere (but not "too far" in the sense of int overflow).

Since Interaction::cellDists holds cell coordinates, it is possible to change the cell boundaries
at runtime. This should make it easy to implement some stress control on the top.

Clumps do not interfere with periodicity in any way.

Rendering
---------
OpenGLRenderingEngine renders InteractingGeometry at all periodic positions that touch the
periodic cell (i.e. BoundingVolume crosses its boundary).

It seems to affect body selection somehow, but that is perhaps not related at all.

Periodicity control
===================
c++:
	World::isPeriodic, World::cellMin, World::cellMax
python:
	O.periodicCell=((0,0,0),(10,10,10)  # activates periodic boundary
	O.periodicCell=() # deactivates periodic boundary

Requirements
============
* No body can have AABB larger than about .499*cellSize. Exception is thrown if that is false.
* Constitutive law must not get body positions from Body::state directly.
	If it does, it uses Interaction::cellDist to compute periodic position.
	Dem3Dof family of Ig2 functors and Law2_* engines are known to behave well.
* No body can get further away than MAXINT periods. It will do horrible things if there is overflow. Not checked at the moment.
* Body cannot move over distance > cellSize in one step. Since body size is limited as well, that would mean simulation explosion.
	Exception is thrown if the movement is upwards. If downwards, it is not handled at all.

Possible performance improvements & bugs
========================================

* PeriodicInsertionSortCollider::{cellWrap,cellWrapRel} OpenGLRenderingEngine::{wrapCell,wrapCellPt} Shop::PeriodicWrap
	are all very similar functions. They should be put into separate header and included from all those places.

*/


#define COLLIDE_STRIDED

// #define this macro to enable timing within this engine
//#define ISC_TIMING

// #define to turn on some tracing information for the periodic part
// all code under this can be probably removed at some point, when the collider will have been tested thoroughly
// #define PISC_DEBUG


#ifdef ISC_TIMING
	#define ISC_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
#else
	#define ISC_CHECKPOINT(cpt)
#endif

class BoundingVolumeDispatcher;
class NewtonsDampedLaw;

class InsertionSortCollider: public Collider{
	//! struct for storing bounds of bodies
	struct Bound{
		//! coordinate along the given sortAxis
		Real coord;
		//! id of the body this bound belongs to
		body_id_t id;
		//! periodic cell coordinate
		int period;
		//! is it the minimum (true) or maximum (false) bound?
		struct{ unsigned hasBB:1; unsigned isMin:1; } flags;
		Bound(Real coord_, body_id_t id_, bool isMin): coord(coord_), id(id_), period(0){ flags.isMin=isMin; }
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
	#ifdef PISC_DEBUG
		bool watchIds(body_id_t id1,body_id_t id2) const;
	#endif
	#ifdef COLLIDE_STRIDED
		// keep this dispatcher and call it ourselves as needed
		shared_ptr<BoundingVolumeDispatcher> boundDispatcher;
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
		int nBins; Real binCoeff, binOverlap, maxRefRelStep; long histInterval; // this last one is debugging-only
	#endif
	struct VecBounds{
		// axis set in the ctor
		int axis;
		std::vector<Bound> vec;
		Real cellMin, cellMax, cellDim;
		// cache vector size(), update at every step in action()
		long size;
		// index of the lowest coordinate element, before which the container wraps
		long loIdx;
		Bound& operator[](long idx){ assert(idx<size && idx>=0); return vec[idx]; }
		const Bound& operator[](long idx) const { assert(idx<size && idx>=0); return vec[idx]; }
		// update number of bodies, periodic properties and size from World
		void updatePeriodicity(World* rb){
			assert(rb->isPeriodic);
			assert(axis>=0 && axis <=2);
			cellMin=rb->cellMin[axis]; cellMax=rb->cellMax[axis]; cellDim=cellMax-cellMin;
		}
		// normalize given index to the right range (wraps around)
		long norm(long i) const { if(i<0) i+=size; long ret=i%size; assert(ret>=0 && ret<size); return ret;}
		VecBounds(): axis(-1), size(0), loIdx(0){}
		void dump(ostream& os){ string ret; for(size_t i=0; i<vec.size(); i++) os<<((long)i==loIdx?"@@ ":"")<<vec[i].coord<<"(id="<<vec[i].id<<","<<(vec[i].flags.isMin?"min":"max")<<",p"<<vec[i].period<<") "; os<<endl;}
	};
	private:
	//! storage for bounds
	VecBounds BB[3];
	//! storage for bb maxima and minima
	std::vector<Real> maxima, minima;
	//! Whether the World was periodic (to detect the change, which shouldn't happen, but shouldn't crash us either)
	bool periodic;



	/*! sorting routine; insertion sort is very fast for strongly pre-sorted lists, which is our case
  	    http://en.wikipedia.org/wiki/Insertion_sort has the algorithm and other details
	*/
	void insertionSort(VecBounds& v,InteractionContainer*,World*,bool doCollide=true);
	void handleBoundInversion(body_id_t,body_id_t,InteractionContainer*,World*);
	bool spatialOverlap(body_id_t,body_id_t) const;

	// periodic variants
	void insertionSortPeri(VecBounds& v,InteractionContainer*,World*,bool doCollide=true);
	void handleBoundInversionPeri(body_id_t,body_id_t,InteractionContainer*,World*);
	bool spatialOverlapPeri(body_id_t,body_id_t,World*,Vector3<int>&) const;
	static Real cellWrap(const Real, const Real, const Real, int&);
	static Real cellWrapRel(const Real, const Real, const Real);


	public:
	//! axis for the initial sort
	int sortAxis;
	//! if true, separate sorting and colliding phase; MUCH slower, but processes all interactions at every step
	// This makes the collider non-persistent, not remembering last state
	bool sortThenCollide;
	//! Predicate called from loop within InteractionContainer::erasePending
	bool shouldBeErased(body_id_t id1, body_id_t id2, World* rb) const {
		if(!periodic) return !spatialOverlap(id1,id2);
		else { Vector3<int> periods; return !spatialOverlapPeri(id1,id2,rb,periods); }
	}
	#ifdef COLLIDE_STRIDED
		virtual bool isActivated(World*);
	#endif

	vector<body_id_t> probeBoundingVolume(const BoundingVolume&);

	InsertionSortCollider():
	#ifdef COLLIDE_STRIDED
		strideActive(false), sweepLength(-1), sweepFactor(1.05), fastestBodyMaxDist(-1), nBins(0), binCoeff(5), binOverlap(0.8), maxRefRelStep(.3), histInterval(100),
	#endif
		periodic(false), sortAxis(0), sortThenCollide(false) {
			#ifdef ISC_TIMING
				timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
			#endif 
			for(int i=0; i<3; i++) BB[i].axis=i;
		 }
	virtual void action(World*);
	REGISTER_CLASS_AND_BASE(InsertionSortCollider,Collider);
	REGISTER_ATTRIBUTES(Collider,(sortAxis)(sortThenCollide)
		#ifdef COLLIDE_STRIDED
			(strideActive)(sweepLength)(sweepFactor)(fastestBodyMaxDist)(nBins)(binCoeff)(binOverlap)(maxRefRelStep)(histInterval)
		#endif
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InsertionSortCollider);
