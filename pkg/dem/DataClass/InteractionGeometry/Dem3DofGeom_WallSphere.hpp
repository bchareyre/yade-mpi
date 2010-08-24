#pragma once
#include<yade/pkg-dem/DemXDofGeom.hpp>
// for static roll/unroll functions in Dem3DofGeom_SphereSphere
#include<yade/pkg-dem/Dem3DofGeom_SphereSphere.hpp>

class Dem3DofGeom_WallSphere: public Dem3DofGeom{

	Vector3r contPtInTgPlane1() const { return se31.position+cp1pt-contactPoint; }
	Vector3r contPtInTgPlane2() const { return Dem3DofGeom_SphereSphere::unrollSpherePtToPlane(se32.orientation*cp2rel,effR2,-normal);}

	public:
		virtual ~Dem3DofGeom_WallSphere();
		/******* API ********/
		virtual Real displacementN(){ return (se32.position-contactPoint).norm()-refLength;}
		virtual Vector3r displacementT(){ relocateContactPoints(); return contPtInTgPlane2()-contPtInTgPlane1(); }
		virtual Real slipToDisplacementTMax(Real displacementTMax);
		virtual Vector3r scaleDisplacementT(Real multiplier);
		/***** end API ******/

		void setTgPlanePts(const Vector3r&, const Vector3r&);
		void relocateContactPoints(){ relocateContactPoints(contPtInTgPlane1(),contPtInTgPlane2()); }
		void relocateContactPoints(const Vector3r& p1, const Vector3r& p2);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Dem3DofGeom_WallSphere,Dem3DofGeom,"Representation of contact between wall and sphere, based on Dem3DofGeom.",
		((Vector3r,cp1pt,,,"initial contact point on the wall, relative to the current contact point"))
		((Quaternionr,cp2rel,,,"orientation between +x and the reference contact point (on the sphere) in sphere-local coords"))
		((Real,effR2,,,"effective radius of sphere")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(Dem3DofGeom_WallSphere,Dem3DofGeom);
	DECLARE_LOGGER;
	friend class Gl1_Dem3DofGeom_WallSphere;
	friend class Ig2_Wall_Sphere_Dem3DofGeom;
};
REGISTER_SERIALIZABLE(Dem3DofGeom_WallSphere);

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class Gl1_Dem3DofGeom_WallSphere:public GlInteractionGeometryFunctor{
		public:
			virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
		RENDERS(Dem3DofGeom_WallSphere);
		YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Dem3DofGeom_WallSphere,GlInteractionGeometryFunctor,"Render interaction of wall and sphere (represented by Dem3DofGeom_WallSphere)",
			((bool,normal,false,,"Render interaction normal"))
			((bool,rolledPoints,false,,"Render points rolled on the spheres (tracks the original contact point)"))
			((bool,unrolledPoints,false,,"Render original contact points unrolled to the contact plane"))
			((bool,shear,false,,"Render shear line in the contact plane"))
			((bool,shearLabel,false,,"Render shear magnitude as number"))
		);
	};
	REGISTER_SERIALIZABLE(Gl1_Dem3DofGeom_WallSphere);
#endif

#include<yade/pkg-common/Dispatching.hpp>
class Ig2_Wall_Sphere_Dem3DofGeom:public InteractionGeometryFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){
			c->swapOrder(); return go(cm2,cm1,state2,state1,-shift2,force,c);
			LOG_ERROR("!! goReverse might not work in Ig2_Wall_Sphere_Dem3DofGeom. InteractionGeometryDispatcher should swap interaction members first and call go(...) afterwards.");
		}

	FUNCTOR2D(Wall,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Wall,Sphere);
	YADE_CLASS_BASE_DOC(Ig2_Wall_Sphere_Dem3DofGeom,InteractionGeometryFunctor,"Create/update contact of :yref:`Wall` and :yref:`Sphere` (:yref:`Dem3DofGeom_WallSphere` instance)");
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Wall_Sphere_Dem3DofGeom);

