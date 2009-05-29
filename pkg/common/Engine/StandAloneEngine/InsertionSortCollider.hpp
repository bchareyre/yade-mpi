// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
class InteractionContainer;
/* Collider that should run in O(n log(n)) time, but faster than PersistentSAPCollider.

	At the initial step, Bodies' bounds (along sortAxis) are first std::sort'ed along one axis (sortAxis), then collided.

	Insertion sort is used for sorting the bound list that is already pre-sorted from last iteration, where each inversion
	calls checkOverlap which then handles either overlap (by creating interaction if necessary) or its absence (by deleting
	interaction if it is only potential).

	Bodies without bounding volume are ahndle gracefully and never collide.


*/

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
		bool operator<(const Bound& b) const {return coord<b.coord;}
		bool operator>(const Bound& b) const {return coord>b.coord;}
	};
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
	bool shouldBeErased(body_id_t id1, body_id_t id2) const { return !spatialOverlap(id1,id2); }

	InsertionSortCollider(): sortAxis(0), sortThenCollide(false){ /* timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);*/ }
	virtual void action(MetaBody*);
	REGISTER_CLASS_AND_BASE(InsertionSortCollider,Collider);
	REGISTER_ATTRIBUTES(Collider,(sortAxis)(sortThenCollide));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InsertionSortCollider);
