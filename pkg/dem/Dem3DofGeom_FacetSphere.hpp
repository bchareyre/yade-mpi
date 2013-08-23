#pragma once
#include<yade/pkg/dem/DemXDofGeom.hpp>
// for static roll/unroll functions in Dem3DofGeom_SphereSphere
#include<yade/pkg/dem/Dem3DofGeom_SphereSphere.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Facet.hpp>

class Dem3DofGeom_FacetSphere: public Dem3DofGeom{
	//! turn planePt in plane with normal0 around line passing through origin to plane with normal1
	static Vector3r turnPlanePt(const Vector3r& planePt, const Vector3r& normal0, const Vector3r& normal1){ Quaternionr q; q.setFromTwoVectors(normal0,normal1); return q*planePt; }

	Vector3r contPtInTgPlane1() const { return turnPlanePt(se31.position+se31.orientation*cp1pt-contactPoint,se31.orientation*localFacetNormal,normal); }
	Vector3r contPtInTgPlane2() const { return Dem3DofGeom_SphereSphere::unrollSpherePtToPlane(se32.orientation*cp2rel,effR2,-normal);}

	public:
		virtual ~Dem3DofGeom_FacetSphere();
		/******* API ********/
		virtual Real displacementN(){ return (se32.position-contactPoint).norm()-refLength;}
		virtual Vector3r displacementT(){ relocateContactPoints(); return contPtInTgPlane2()-contPtInTgPlane1(); }
		virtual Real slipToDisplacementTMax(Real displacementTMax);
		/***** end API ******/

		void setTgPlanePts(const Vector3r&, const Vector3r&);
		void relocateContactPoints(){ relocateContactPoints(contPtInTgPlane1(),contPtInTgPlane2()); }
		void relocateContactPoints(const Vector3r& p1, const Vector3r& p2);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Dem3DofGeom_FacetSphere,Dem3DofGeom,"Class representing facet+sphere in contact which computes 3 degrees of freedom (normal and shear deformation).",
		((Vector3r,cp1pt,,,"Reference contact point on the facet in facet-local coords."))
		((Quaternionr,cp2rel,,,"Orientation between +x and the reference contact point (on the sphere) in sphere-local coords"))
		((Vector3r,localFacetNormal,,,"Unit normal of the facet plane in facet-local coordinates"))
		((Real,effR2,,,"Effective radius of sphere")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(Dem3DofGeom_FacetSphere,Dem3DofGeom);
	DECLARE_LOGGER;
	friend class Gl1_Dem3DofGeom_FacetSphere;
	friend class Ig2_Facet_Sphere_Dem3DofGeom;
};
REGISTER_SERIALIZABLE(Dem3DofGeom_FacetSphere);

#ifdef YADE_OPENGL
	#include<yade/pkg/common/GLDrawFunctors.hpp>
	class Gl1_Dem3DofGeom_FacetSphere:public GlIGeomFunctor{
		public:
			virtual void go(const shared_ptr<IGeom>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
		RENDERS(Dem3DofGeom_FacetSphere);
		YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Dem3DofGeom_FacetSphere,GlIGeomFunctor,"Render interaction of facet and sphere (represented by Dem3DofGeom_FacetSphere)",
			((bool,normal,false,,"Render interaction normal"))
			((bool,rolledPoints,false,,"Render points rolled on the sphere & facet (original contact point)"))
			((bool,unrolledPoints,false,,"Render original contact points unrolled to the contact plane"))
			((bool,shear,false,,"Render shear line in the contact plane"))
			((bool,shearLabel,false,,"Render shear magnitude as number"))
		);
	};
	REGISTER_SERIALIZABLE(Gl1_Dem3DofGeom_FacetSphere);
#endif

#include<yade/pkg/common/Dispatching.hpp>
class Ig2_Facet_Sphere_Dem3DofGeom:public IGeomFunctor{
	Vector3r getClosestSegmentPt(const Vector3r& P, const Vector3r& A, const Vector3r& B){
		// algo: http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
		Vector3r BA=B-A;
		Real u=(P.dot(BA)-A.dot(BA))/(BA.squaredNorm());
		return A+min((Real)1.,max((Real)0.,u))*BA;
	}
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
			c->swapOrder(); return go(cm2,cm1,state2,state1,-shift2,force,c);
			LOG_ERROR("!! goReverse maybe doesn't work in Ig2_Facet_Sphere_Dem3DofGeom. IGeomDispatcher should swap interaction members first and call go(...) afterwards.");
		}

	FUNCTOR2D(Facet,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Facet,Sphere);
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Facet_Sphere_Dem3DofGeom,IGeomFunctor,"Compute geometry of facet-sphere contact with normal and shear DOFs. As in all other Dem3DofGeom-related classes, total formulation of both shear and normal deformations is used. See :yref:`Dem3DofGeom_FacetSphere` for more information.",
		// unused: ((Real,shrinkFactor,0.,,"Reduce the facet's size, probably to avoid singularities at common facets' edges (?)"))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Facet_Sphere_Dem3DofGeom);

