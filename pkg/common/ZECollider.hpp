// 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>

#pragma once
#include<yade/pkg/common/Collider.hpp>
#include<yade/core/Scene.hpp>
class InteractionContainer;



// #define this macro to enable timing within this engine
#define ISC_TIMING

#ifdef ISC_TIMING
	#define ISC_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
#else
	#define ISC_CHECKPOINT(cpt)
#endif

class NewtonIntegrator;



#include <CGAL/box_intersection_d.h>
#include <CGAL/Bbox_2.h>
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/intersections.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/box_intersection_d.h>
#include <CGAL/function_objects.h>
#include <CGAL/Join_input_iterator.h>
#include <CGAL/algorithm.h>
#include <vector>

typedef CGAL::Bbox_3                              CGBbox;
// typedef CGAL::Box_intersection_d::Box_with_handle_d<double,3,shared_ptr<Body> > CGBox;

typedef CGAL::Box_intersection_d::Box_with_handle_d<double,3,shared_ptr<Body> > CGBox_noId;
class CGBox: public CGBox_noId {
	public:
		Body::id_t bid;
		CGBox(CGBbox box, shared_ptr<Body>& body): CGBox_noId(box, body) {}
	
};

// class CGBox;

class ZECollider: public Collider{
	//! struct for storing bounds of bodies
	std::vector<CGBox> boxes;

		// we need this to find out about current maxVelocitySq
		shared_ptr<NewtonIntegrator> newton;
		// if False, no type of striding is used
		// if True, then either verletDist XOR nBins is set
		bool strideActive;
		bool periodic;
	private:
	static InteractionContainer* interactions;
	static Scene* sscene;

	static void handleOverlap(const CGBox& a, const CGBox& b);

	//FIXME: this will not work if bodies are not numbered from 0 to N with increment 1
	inline bool spatialOverlap(const Body::id_t& id1, const Body::id_t& id2) const {
		assert(!scene->isPeriodic);
		assert(boxes.size());
		const CGBox& b1 = boxes[id1];
		const CGBox& b2 = boxes[id2];
		return	(b1.min_coord(0)<=b2.max_coord(0)) && (b1.max_coord(0)>=b2.min_coord(0)) &&
			(b1.min_coord(1)<=b2.max_coord(1)) && (b1.max_coord(1)>=b2.min_coord(1)) &&
			(b1.min_coord(2)<=b2.max_coord(2)) && (b1.max_coord(2)>=b2.min_coord(2));
	}
	
	public:
	//! Predicate called from loop within InteractionContainer::conditionalyEraseNonReal()
	bool shouldBeErased(Body::id_t id1, Body::id_t id2, Scene* rb) const {return !spatialOverlap(id1,id2);}
	
	virtual bool isActivated();

	// force reinitialization at next run
	virtual void invalidatePersistentData(){}

// 	vector<Body::id_t> probeBoundingVolume(const Bound&);

	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(ZECollider,Collider,"\
		Collider with O(n log(n)) complexity, using a CGAL algorithm from Zomorodian and Edelsbrunner [Kettner2011]_ (http://www.cgal.org/Manual/beta/doc_html/cgal_manual/Box_intersection_d/Chapter_main.html)",
		((int,sortAxis,0,,"Axis for the initial contact detection."))
		((bool,sortThenCollide,false,,"Separate sorting and colliding phase; it is MUCH slower, but all interactions are processed at every step; this effectively makes the collider non-persistent, not remembering last state. (The default behavior relies on the fact that inversions during insertion sort are overlaps of bounding boxes that just started/ceased to exist, and only processes those; this makes the collider much more efficient.)"))
// 		((bool,oriVerlet,true,,"Compare Verlet distance with displacement instead of velocity (only used if nBins<=0)"))
		((int,targetInterv,30,,"(experimental) Target number of iterations between bound update, used to define a smaller sweep distance for slower grains if >0, else always use 1*verletDist. Useful in simulations with strong velocity contrasts between slow bodies and fast bodies."))
		((Real,updatingDispFactor,-1,,"(experimental) Displacement factor used to trigger bound update: the bound is updated only if updatingDispFactor*disp>sweepDist when >0, else all bounds are updated."))
		((Real,verletDist,((void)"Automatically initialized",-.15),,"Length by which to enlarge particle bounds, to avoid running collider at every step. Stride disabled if zero. Negative value will trigger automatic computation, so that the real value will be |verletDist| × minimum spherical particle radius; if there are no spherical particles, it will be disabled."))
		((Real,fastestBodyMaxDist,-1,,"Maximum displacement of the fastest body since last run; if >= verletDist, we could get out of bboxes and will trigger full run. DEPRECATED, was only used without bins. |yupdate|"))
		((int,numReinit,0,Attr::readonly,"Cummulative number of bound array re-initialization."))
		, /*deprec*/ ((sweepLength,verletDist,"conform to usual DEM terminology"))
		, /* init */
		,
		/* ctor */
			#ifdef ISC_TIMING
				timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
			#endif 
			periodic=false;
			strideActive=false;
			,
		/* py */
		.def_readonly("strideActive",&ZECollider::strideActive,"Whether striding is active (read-only; for debugging). |yupdate|")
		.def_readonly("periodic",&ZECollider::periodic,"Whether the collider is in periodic mode (read-only; for debugging) |yupdate|")
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(ZECollider);
