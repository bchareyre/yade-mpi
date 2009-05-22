// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
class InteractionContainer;
/* Collider that should run in O(n log(n)) time, but faster than PersistentSAPCollider.

	At the initial step, Bodies' bounds (along sortAxis) are first std::sort'ed along one axis (sortAxis), then collided.

	Insertion sort is used for sorting the bound list that is already pre-sorted from last iteration, where each inversion
	calls checkOverlap which then handles either overlap (by creating interaction if necessary) or its absence (by deleting
	interaction if it exists and is only potential (!isReal && isNew).

	Note that not all interactions are traversed at one run, therefore an overlap miss also has to check the interaction container.

	For performance reasons, we require all bodies to have boundingVolume.

*/

class InsertionSortCollider: public Collider{
	//! struct for storing bounds of bodies
	struct Bound{
		//! coordinate along the given sortAxis
		Real coord;
		//! id of the body this bound belongs to
		body_id_t id;
		//! is it the minimum (true) or maximum (false) bound?
		bool isMin;
		Bound(Real coord_, body_id_t id_, bool isMin_): coord(coord_), id(id_), isMin(isMin_){}
		//Bound(const Bound& b): coord(b.coord), id(b.id), isMin(b.isMin){}
		//Bound& operator=(const Bound& b){ coord=b.coord; id=b.id; isMin=b.isMin; cerr<<"!=!"<<endl; return *this;}
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
