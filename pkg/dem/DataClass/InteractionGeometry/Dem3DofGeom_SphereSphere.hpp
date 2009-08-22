#pragma once
#include<yade/pkg-dem/DemXDofGeom.hpp>

class Dem3DofGeom_SphereSphere: public Dem3DofGeom{
	public:
		// auxiliary functions; the quaternion magic is all in here
		static Vector3r unrollSpherePtToPlane(const Quaternionr& fromXtoPtOri, const Real& radius, const Vector3r& normal);
		static Quaternionr rollPlanePtToSphere(const Vector3r& planePt, const Real& radius, const Vector3r& normal);
	private:
		Vector3r contPtInTgPlane1() const { return unrollSpherePtToPlane(ori1*cp1rel,effR1,normal); }
		Vector3r contPtInTgPlane2() const { return unrollSpherePtToPlane(ori2*cp2rel,effR2,-normal);}
		void setTgPlanePts(Vector3r p1new, Vector3r p2new);
		void relocateContactPoints();
		void relocateContactPoints(const Vector3r& tgPlanePt1, const Vector3r& tgPlanePt2);
	public:
		//! relative orientation of the contact point with regards to sphere-local +x axis (quasi-constant)
		Quaternionr cp1rel, cp2rel;
		//! shorthands
		const Vector3r &pos1; const Quaternionr &ori1; const Vector3r &pos2; const Quaternionr &ori2;
		Dem3DofGeom_SphereSphere(): pos1(se31.position), ori1(se31.orientation), pos2(se32.position), ori2(se32.orientation){ createIndex(); }
		virtual ~Dem3DofGeom_SphereSphere();
		//! effective radii of spheres for this interaction; can be smaller/larger than actual radii, but quasi-constant throughout the interaction life
		Real effR1, effR2;
		
		/********* API **********/
		Real displacementN(){ return (pos2-pos1).Length()-refLength; }
		Vector3r displacementT() {
			// enabling automatic relocation decreases overall simulation speed by about 3%; perhaps: bool largeStrains ... ?
			#if 1 
				Vector3r p1=contPtInTgPlane1(), p2=contPtInTgPlane2(); relocateContactPoints(p1,p2); return p2-p1; // shear before relocation, but that is OK
			#else
				return contPtInTgPlane2()-contPtInTgPlane1();
			#endif
		}
		Real slipToDisplacementTMax(Real displacementTMax);
		/********* end API ***********/

	REGISTER_ATTRIBUTES(Dem3DofGeom,(effR1)(effR2)(cp1rel)(cp2rel));
	REGISTER_CLASS_AND_BASE(Dem3DofGeom_SphereSphere,Dem3DofGeom);
	REGISTER_CLASS_INDEX(Dem3DofGeom_SphereSphere,Dem3DofGeom);
	friend class GLDraw_Dem3DofGeom_SphereSphere;
	friend class ef2_Sphere_Sphere_Dem3DofGeom;
};
REGISTER_SERIALIZABLE(Dem3DofGeom_SphereSphere);

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class GLDraw_Dem3DofGeom_SphereSphere:public GLDrawInteractionGeometryFunctor{
		public:
			virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
			static bool normal,rolledPoints,unrolledPoints,shear,shearLabel;
		//RENDERS(Dem3DofGeom_SphereSphere);
		//REGISTER_CLASS_AND_BASE(GLDraw_Dem3DofGeom_SphereSphere,GLDrawInteractionGeometryFunctor);
		REGISTER_ATTRIBUTES(GLDrawInteractionGeometryFunctor,(normal)(rolledPoints)(unrolledPoints)(shear)(shearLabel));
	};
	REGISTER_SERIALIZABLE(GLDraw_Dem3DofGeom_SphereSphere);
#endif

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>
class ef2_Sphere_Sphere_Dem3DofGeom:public InteractionGeometryEngineUnit{
	public:
		virtual bool go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>&, const shared_ptr<InteractingGeometry>&, const Se3r&, const Se3r&, const shared_ptr<Interaction>&){throw runtime_error("goReverse on symmetric functor should never be called!");}
		//! Factor of sphere radius such that sphere "touch" if their centers are not further than distFactor*(r1+r2);
		//! if negative, equilibrium distance is the sum of the sphere's radii, which is the default.
		Real distFactor;
		ef2_Sphere_Sphere_Dem3DofGeom(): distFactor(-1.) {}
	FUNCTOR2D(InteractingSphere,InteractingSphere);
	DEFINE_FUNCTOR_ORDER_2D(InteractingSphere,InteractingSphere);
	REGISTER_CLASS_AND_BASE(ef2_Sphere_Sphere_Dem3DofGeom,InteractionGeometryEngineUnit);
	REGISTER_ATTRIBUTES(InteractionGeometryEngineUnit,(distFactor));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(ef2_Sphere_Sphere_Dem3DofGeom);

