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
		//! shorthands
		const Vector3r &pos1; const Quaternionr &ori1; const Vector3r &pos2; const Quaternionr &ori2;
		virtual ~Dem3DofGeom_SphereSphere();
		
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
		Real penetrationDepth() {return (refR1+refR2)-refLength-displacementN();}
		/********* end API ***********/

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(Dem3DofGeom_SphereSphere,Dem3DofGeom,"Class representing 2 spheres in contact which computes 3 degrees of freedom (normal and shear deformation).",
		((Real,effR1,,"Effective radius of sphere #1; can be smaller/larger than refR1 (the actual radius), but quasi-constant throughout interaction life"))
		((Real,effR2,,"Same as effR1, but for sphere #2."))
		((Quaternionr,cp1rel,,"Sphere's #1 relative orientation of the contact point with regards to sphere-local +x axis (quasi-constant)"))
		((Quaternionr,cp2rel,,"Same as cp1rel, but for sphere #2.")),
		/* extra initializers */ ((pos1,se31.position))((ori1,se31.orientation))((pos2,se32.position))((ori2,se32.orientation)),
		/*ctor*/ createIndex(); ,
		/*py*/
	);
	REGISTER_CLASS_INDEX(Dem3DofGeom_SphereSphere,Dem3DofGeom);
	friend class Gl1_Dem3DofGeom_SphereSphere;
	friend class Ig2_Sphere_Sphere_Dem3DofGeom;
};
REGISTER_SERIALIZABLE(Dem3DofGeom_SphereSphere);

class Dem6DofGeom_SphereSphere: public Dem3DofGeom_SphereSphere{
	public:
	// return relative rotation, composed of both bend and twist
	Vector3r relRotVector() const;
	virtual void bendTwistAbs(Vector3r& bend, Real& twist);
	virtual ~Dem6DofGeom_SphereSphere();
	Dem6DofGeom_SphereSphere(const Dem3DofGeom_SphereSphere& ss): Dem3DofGeom_SphereSphere(ss){ createIndex(); }
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Dem6DofGeom_SphereSphere,Dem3DofGeom_SphereSphere,"Class representing 2 sphere in contact which computes 6 degrees of freedom (normal, shear, bending and twisting deformation)",
		((Quaternionr,initRelOri12,,"Initial relative orientation of spheres, used for bending and twisting computation.")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(Dem6DofGeom_SphereSphere,Dem3DofGeom_SphereSphere);
};
REGISTER_SERIALIZABLE(Dem6DofGeom_SphereSphere);

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class Gl1_Dem3DofGeom_SphereSphere:public GlInteractionGeometryFunctor{
		public:
			virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
		RENDERS(Dem3DofGeom_SphereSphere);
		YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Dem3DofGeom_SphereSphere,GlInteractionGeometryFunctor,"Render interaction of 2 spheres (represented by Dem3DofGeom_SphereSphere)",
			((bool,normal,false,"Render interaction normal"))
			((bool,rolledPoints,false,"Render points rolled on the spheres (tracks the original contact point)"))
			((bool,unrolledPoints,false,"Render original contact points unrolled to the contact plane"))
			((bool,shear,false,"Render shear line in the contact plane"))
			((bool,shearLabel,false,"Render shear magnitude as number"))
		);
	};
	REGISTER_SERIALIZABLE(Gl1_Dem3DofGeom_SphereSphere);
#endif

#include<yade/pkg-common/InteractionGeometryFunctor.hpp>
class Ig2_Sphere_Sphere_Dem3DofGeom:public InteractionGeometryFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>&, const shared_ptr<Shape>&, const State&, const State&, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>&){throw runtime_error("goReverse on symmetric functor should never be called!");}
	FUNCTOR2D(Sphere,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_Dem3DofGeom,InteractionGeometryFunctor,
		"Functor handling contact of 2 spheres, producing Dem3DofGeom instance",
		((Real,distFactor,-1,"Factor of sphere radius such that sphere \"touch\" if their centers are not further than distFactor*(r1+r2); if negative, equilibrium distance is the sum of the sphere's radii."))
	);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_Dem3DofGeom);

class Ig2_Sphere_Sphere_Dem6DofGeom: public Ig2_Sphere_Sphere_Dem3DofGeom{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>&, const shared_ptr<Shape>&, const State&, const State&, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>&){throw runtime_error("goReverse on symmetric functor should never be called!");}
	FUNCTOR2D(Sphere,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
	YADE_CLASS_BASE_DOC(Ig2_Sphere_Sphere_Dem6DofGeom,Ig2_Sphere_Sphere_Dem3DofGeom,"Create/update contact of 2 spheres with 6 DOFs (:yref:`Dem6DofGeom_SphereSphere` instance) [experimental]");
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_Dem6DofGeom);
