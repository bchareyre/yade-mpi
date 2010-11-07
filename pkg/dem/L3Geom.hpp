
#pragma once
#include<yade/core/IGeom.hpp>
#include<yade/core/IPhys.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/State.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/dem/DemXDofGeom.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>

struct L3Geom: public GenericSpheresContact{
	const Real& uN;
	const Vector2r& uT; 
	virtual ~L3Geom();

	// utility function
	// TODO: currently supposes body's centroids are conencted with distance*normal
	// that will not be true for sphere+facet and others, watch out!
	// the force is oriented as applied to particle #1
	void applyLocalForce(const Vector3r& f, const Interaction* I, Scene* scene, NormShearPhys* nsp=NULL) const;
	void applyLocalForceTorque(const Vector3r& f, const Vector3r& t, const Interaction* I, Scene* scene, NormShearPhys* nsp=NULL) const;

	Vector3r relU() const{ return u-u0; }

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(L3Geom,GenericSpheresContact,"Geometry of contact given in local coordinates with 3 degress of freedom: normal and two in shear plane. [experimental]",
		((Vector3r,u,Vector3r::Zero(),,"Displacement components, in local coordinates. |yupdate|"))
		((Vector3r,u0,Vector3r::Zero(),,"Zero displacement value; u0 should be always subtracted from the *geometrical* displacement *u* computed by appropriate :yref:`IGeomFunctor`, resulting in *u*. This value can be changed for instance\n\n#. by :yref:`IGeomFunctor`, e.g. to take in account large shear displacement value unrepresentable by underlying geomeric algorithm based on quaternions)\n#. by :yref:`LawFunctor`, to account for normal equilibrium position different from zero geometric overlap (set once, just after the interaction is created)\n#. by :yref:`LawFunctor` to account for plastic slip.\n\n.. note:: Never set an absolute value of *u0*, only increment, since both :yref:`IGeomFunctor` and :yref:`LawFunctor` use it. If you need to keep track of plastic deformation, store it in :yref:`IPhys` isntead (this might be changed: have *u0* for :yref:`LawFunctor` exclusively, and a separate value stored (when that is needed) inside classes deriving from :yref:`L3Geom`."))
		/* Is it better to store trsf as Matrix3 or Quaternion?
			* Quaternions are much easier to re-normalize, which we should do to avoid numerical drift.
			* Multiplication of vector with quaternion is internally done by converting to matrix first, anyway
			* We need to extract local axes, and that is easier to be done from Matrix3r (columns)
		*/
		((Matrix3r,trsf,Matrix3r::Identity(),,"Transformation (rotation) from global to local coordinates. (the translation part is in :yref:`GenericSpheresContact.contactPoint`)"))
		,
		/*init*/
		((uN,u[0])) ((uT,Vector2r::Map(&u[1])))
		,
		/*ctor*/ createIndex();
		, /*py*/
		//.def_readonly("uN",&L3Geom::uN,"Normal component of *u*")
		//.def_readonly("uT",&L3Geom::uT,"Shear component of *u*")
	);
	REGISTER_CLASS_INDEX(L3Geom,GenericSpheresContact);
};
REGISTER_SERIALIZABLE(L3Geom);


struct L6Geom: public L3Geom{
	virtual ~L6Geom();
	Vector3r relPhi() const{ return phi-phi0; }
	YADE_CLASS_BASE_DOC_ATTRS(L6Geom,L3Geom,"Geoemtric of contact in local coordinates with 6 degrees of freedom. [experimental]",
		((Vector3r,phi,Vector3r::Zero(),,"Rotation components, in local coordinates. |yupdate|"))
		((Vector3r,phi0,Vector3r::Zero(),,"Zero rotation, should be always subtracted from *phi* to get the value. See :yref:`L3Geom.u0`."))
	);
};
REGISTER_SERIALIZABLE(L6Geom);


struct Ig2_Sphere_Sphere_L3Geom_Inc: public IGeomFunctor{
		virtual bool go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I);
		virtual bool genericGo(bool is6Dof, const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I);

	enum { APPROX_NO_RENORM_TRSF=1, APPROX_NO_MID_TRSF=2, APPROX_NO_MID_NORMAL=4, APPROX_NO_RENORM_MID_NORMAL=8 };

	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_L3Geom_Inc,IGeomFunctor,"Incrementally compute :yref:`L3Geom` for contact of 2 spheres.\n\n.. note:: The initial value of *u[0]* (normal displacement) might be non-zero, with or without *distFactor*, since it is given purely by sphere's geometry. If you want to set \"equilibrium distance\", do it in the contact law as explained in :yref:`L3Geom.u0`.",
		((bool,noRatch,true,,"See :yref:`ScGeom.avoidGranularRatcheting`."))
		((Real,distFactor,1,,"Create interaction if spheres are not futher than distFactor*(r1+r2)."))
		((int,approxMask,0,,"Selectively enable geometrical approximations (bitmask); add the values for approximations to be enabled.\n\n1: do not renormalize transformation matrix at every step\n2: use previous transformation to transform velocities (which are known at mid-steps), instead of mid-step transformation computed as quaternion slerp at t=0.5.\n4: do not take average (mid-step) normal when computing relative shear displacement, use previous value instead\n8: do not re-normalize average (mid-step) normal, if used.…\nBy default, the mask is zero and neither of these approximations is used."))
	);
	FUNCTOR2D(Sphere,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_L3Geom_Inc);


struct Ig2_Sphere_Sphere_L6Geom_Inc: public Ig2_Sphere_Sphere_L3Geom_Inc{
	virtual bool go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I);
	YADE_CLASS_BASE_DOC(Ig2_Sphere_Sphere_L6Geom_Inc,Ig2_Sphere_Sphere_L3Geom_Inc,"Incrementally compute :yref:`L6Geom` for contact of 2 spheres.");
	FUNCTOR2D(Sphere,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_L6Geom_Inc);


struct Law2_L3Geom_FrictPhys_ElPerfPl: public LawFunctor{
	virtual void go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);
	FUNCTOR2D(L3Geom,FrictPhys);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_L3Geom_FrictPhys_ElPerfPl,LawFunctor,"Basic law for testing :yref:`L3Geom`; it bears no cohesion (unless *noBreak* is ``True``), and plastic slip obeys the Mohr-Coulomb criterion (unless *noSlip* is ``True``).",
		((bool,noBreak,false,,"Do not break contacts when particles separate."))
		((bool,noSlip,false,,"No plastic slipping."))
	);
};
REGISTER_SERIALIZABLE(Law2_L3Geom_FrictPhys_ElPerfPl);

struct Law2_L6Geom_FrictPhys_Linear: public Law2_L3Geom_FrictPhys_ElPerfPl{
	virtual void go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);
	FUNCTOR2D(L6Geom,FrictPhys);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_L6Geom_FrictPhys_Linear,Law2_L3Geom_FrictPhys_ElPerfPl,"Basic law for testing :yref:`L6Geom` -- linear in both normal and shear sense, without slip or breakage.",
		((Real,charLen,1,,"Characteristic length with the meaning of the stiffness ratios bending/shear and torsion/normal."))
	);
};
REGISTER_SERIALIZABLE(Law2_L6Geom_FrictPhys_Linear);
