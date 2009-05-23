// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
class InteractionContainer;
/* Collider that should run in O(n log(n)) time, but faster than PersistentSAPCollider.

	At the initial step, Bodies' bounds (along sortAxis) are first std::sort'ed along one axis (sortAxis), then collided.

	Insertion sort is used for sorting the bound list that is already pre-sorted from last iteration, where each inversion
	calls checkOverlap which then handles either overlap (by creating interaction if necessary) or its absence (by deleting
	interaction if it exists and is only potential (!isReal && isNew).

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
		char type;
		Bound(Real coord_, body_id_t id_, char type_): coord(coord_), id(id_), type(type_){}
		bool operator<(const Bound& b) const {return coord<b.coord;}
		bool operator>(const Bound& b) const {return coord>b.coord;}
		enum { FLAG_MIN=1, FLAG_BB=2 };
		inline bool hasBB() const {return type&FLAG_BB;}
		inline bool isMin() const {return type&FLAG_MIN;}
		inline void setBB(){type|=FLAG_BB;}
		inline void setNoBB(){type&=type^FLAG_BB;}
	};
	//! storage for bounds
	std::vector<Bound> XX,YY,ZZ;
	//! storage for bb maxima and minima
	std::vector<Real> maxima, minima;



	/*! sorting routine; insertion sort is very fast for strongly pre-sorted lists, which is our case
  	    http://en.wikipedia.org/wiki/Insertion_sort has the algorithm and other details
	*/
	void insertionSort(std::vector<Bound>& v,InteractionContainer*,MetaBody*);
	void handleBoundInversion(body_id_t,body_id_t,InteractionContainer*,MetaBody*);
	bool spatialOverlap(body_id_t,body_id_t);

	public:
	//! axis for the initial sort
	int sortAxis;

	InsertionSortCollider(): sortAxis(0){ /* timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);*/ }
	virtual void action(MetaBody*);
	REGISTER_CLASS_AND_BASE(InsertionSortCollider,Collider);
	REGISTER_ATTRIBUTES(Collider,(sortAxis));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InsertionSortCollider);
