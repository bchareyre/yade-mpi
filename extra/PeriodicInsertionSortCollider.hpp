// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once
#include<yade/core/Collider.hpp>
class InteractionContainer;

class PeriodicInsertionSortCollider: public Collider{
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
		bool operator<(const Bound& b) const {return coord<b.coord;}
		bool operator>(const Bound& b) const {return coord>b.coord;}
	};
	struct VecBounds{
		std::vector<Bound> vec;
		Real cellMin, cellMax, cellDim;
		long size;
		// index of the lowest coordinate element, before which the container wraps
		long loIdx;
		Bound& operator[](long idx){ assert(idx<size && idx>=0); return vec[idx]; }
		const Bound& operator[](long idx) const { assert(idx<size && idx>=0); return vec[idx]; }
		// update periodic properties and size from MetaBody
		void update(MetaBody* rb, int axis);
		// normalize given index to the right range (wraps around)
		long norm(long i) const { if(i<0) i+=size; long ret=i%size; assert(ret>=0 && ret<size); return ret;}
		VecBounds(): size(0), loIdx(0){}
		void dump(ostream& os){ string ret; for(size_t i=0; i<vec.size(); i++) os<<((long)i==loIdx?"@@ ":"")<<vec[i].coord<<"(id="<<vec[i].id<<","<<(vec[i].flags.isMin?"min":"max")<<",p"<<vec[i].period<<") "; os<<endl;}
	};
	private:
	//! storage for bounds
	VecBounds XX,YY,ZZ;
	//! storage for bb maxima and minima
	std::vector<Real> maxima, minima;

	void insertionSort(VecBounds& v,InteractionContainer*,MetaBody*,bool doCollide=true);
	void handleBoundInversion(body_id_t,body_id_t,InteractionContainer*,MetaBody*);
	bool spatialOverlap(body_id_t,body_id_t, MetaBody*, Vector3<int>&) const;
	static Real cellWrap(Real,Real,Real,long&);

	public:
	//! axis for the initial sort
	int sortAxis;
	//! Predicate called from loop within InteractionContainer::erasePending
	bool shouldBeErased(body_id_t id1, body_id_t id2, MetaBody* rb) const { Vector3<int> foo; return !spatialOverlap(id1,id2,rb,foo); }

	PeriodicInsertionSortCollider(): sortAxis(0) { }
	virtual void action(MetaBody*);
	REGISTER_CLASS_AND_BASE(PeriodicInsertionSortCollider,Collider);
	REGISTER_ATTRIBUTES(Collider,(sortAxis));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PeriodicInsertionSortCollider);
