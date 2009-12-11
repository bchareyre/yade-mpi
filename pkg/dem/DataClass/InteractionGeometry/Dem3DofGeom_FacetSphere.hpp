#pragma once
#include<yade/pkg-dem/DemXDofGeom.hpp>
// for static roll/unroll functions in Dem3DofGeom_SphereSphere
#include<yade/pkg-dem/Dem3DofGeom_SphereSphere.hpp>

class Dem3DofGeom_FacetSphere: public Dem3DofGeom{
	//! turn planePt in plane with normal0 around line passing through origin to plane with normal1
	static Vector3r turnPlanePt(const Vector3r& planePt, const Vector3r& normal0, const Vector3r& normal1){ Quaternionr q; q.Align(normal0,normal1); return q*planePt; }

	Vector3r contPtInTgPlane1() const { return turnPlanePt(se31.position+se31.orientation*cp1pt-contactPoint,se31.orientation*localFacetNormal,normal); }
	Vector3r contPtInTgPlane2() const { return Dem3DofGeom_SphereSphere::unrollSpherePtToPlane(se32.orientation*cp2rel,effR2,-normal);}

	public:
		Dem3DofGeom_FacetSphere(){ createIndex();}
		virtual ~Dem3DofGeom_FacetSphere();
		/******* API ********/
		virtual Real displacementN(){ return (se32.position-contactPoint).Length()-refLength;}
		virtual Vector3r displacementT(){ relocateContactPoints(); return contPtInTgPlane2()-contPtInTgPlane1(); }
		virtual Real slipToDisplacementTMax(Real displacementTMax);
		/***** end API ******/

		void setTgPlanePts(const Vector3r&, const Vector3r&);
		void relocateContactPoints(){ relocateContactPoints(contPtInTgPlane1(),contPtInTgPlane2()); }
		void relocateContactPoints(const Vector3r& p1, const Vector3r& p2);
	//! reference contact point on the facet in facet-local coords
	Vector3r cp1pt;
	//! orientation between +x and the reference contact point (on the sphere) in sphere-local coords
	Quaternionr cp2rel;
	//! unit normal of the facet plane in facet-local coordinates
	Vector3r localFacetNormal;
	// effective radius of sphere
	Real effR2;
	REGISTER_ATTRIBUTES(Dem3DofGeom,(cp1pt)(cp2rel)(localFacetNormal)(effR2) );
	REGISTER_CLASS_AND_BASE(Dem3DofGeom_FacetSphere,Dem3DofGeom);
	REGISTER_CLASS_INDEX(Dem3DofGeom_FacetSphere,Dem3DofGeom);
	DECLARE_LOGGER;
	friend class Gl1_Dem3DofGeom_FacetSphere;
	friend class Ig2_Facet_Sphere_Dem3DofGeom;
};
REGISTER_SERIALIZABLE(Dem3DofGeom_FacetSphere);

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class Gl1_Dem3DofGeom_FacetSphere:public GlInteractionGeometryFunctor{
		public:
			virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
			static bool normal,rolledPoints,unrolledPoints,shear,shearLabel;
		RENDERS(Dem3DofGeom_FacetSphere);
		REGISTER_CLASS_AND_BASE(Gl1_Dem3DofGeom_FacetSphere,GlInteractionGeometryFunctor);
		REGISTER_ATTRIBUTES(GlInteractionGeometryFunctor, (normal)(rolledPoints)(unrolledPoints)(shear)(shearLabel) );
	};
	REGISTER_SERIALIZABLE(Gl1_Dem3DofGeom_FacetSphere);
#endif

#include<yade/pkg-common/InteractionGeometryFunctor.hpp>
class Ig2_Facet_Sphere_Dem3DofGeom:public InteractionGeometryFunctor{
	Vector3r getClosestSegmentPt(const Vector3r& P, const Vector3r& A, const Vector3r& B){
		// algo: http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
		Vector3r BA=B-A;
		Real u=(P.Dot(BA)-A.Dot(BA))/(BA.SquaredLength());
		return A+min(1.,max(0.,u))*BA;
	}
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
			c->swapOrder(); return go(cm2,cm1,state2,state1,-shift2,force,c);
			LOG_ERROR("!! goReverse maybe doesn't work in Ig2_Facet_Sphere_Dem3DofGeom. InteractionGeometryDispatcher should swap interaction members first and call go(...) afterwards.");
		}

		//! Reduce the facet's size, probably to avoid singularities at common facets' edges (?)
		Real shrinkFactor;
		Ig2_Facet_Sphere_Dem3DofGeom(): shrinkFactor(0.) {}
	FUNCTOR2D(InteractingFacet,InteractingSphere);
	DEFINE_FUNCTOR_ORDER_2D(InteractingFacet,InteractingSphere);
	REGISTER_CLASS_AND_BASE(Ig2_Facet_Sphere_Dem3DofGeom,InteractionGeometryFunctor);
	REGISTER_ATTRIBUTES(InteractionGeometryFunctor,(shrinkFactor));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Facet_Sphere_Dem3DofGeom);

