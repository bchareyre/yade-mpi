
#include<yade/core/IGeom.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/State.hpp>
#include<yade/pkg-common/Dispatching.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>

class L3Geom: public GenericSpheresContact{
	virtual ~L3Geom();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(L3Geom,GenericSpheresContact,"Geometry of contact given in local coordinates with 3 degress of freedom: normal and two in shear plane. [experimental]",
		((Vector3r,u,Vector3r::Zero(),,"Displacement components, in local coordinates. |yupdate|"))
		((Vector3r,u0,Vector3r::Zero(),,"Zero displacement value; u0 will be always subtracted from the *geometrical* displacement by appropriate :yref:`IGeomFunctor`, resulting in *u*. This value can be changed for instance\n\n#. by :yref:`IGeomFunctor`, e.g. to take in account large shear displacement value unrepresentable by underlying geomeric algorithm based on quaternions)\n#. by :yref:`LawFunctor`, to account for normal equilibrium position different from zero geometric overlap (set once, just after the interaction is created)\n#. by :yref:`LawFunctor` to account for plastic slip.\n\n.. note:: Never set an absolute value of *u0*, only increment, since both :yref:`IGeomFunctor` and :yref:`LawFunctor` use it. If you need to keep track of plastic deformation, store it in :yref:`IPhys` isntead (this might be changed: have *u0* for :yref:`LawFunctor` exclusively, and a separate value stored (when that is needed) inside classes deriving from :yref:`L3Geom`."))
		/* Is it better to store trsf as Matrix3 or Quaternion?
			* Quaternions are much easier to re-normalize, which we should do to avoid numerical drift.
			* Multiplication of vector with quaternion is internally done by converting to matrix first, anyway
			* We need to extract local axes, and that is easier to be done from Matrix3r (columns)
		*/
		((Matrix3r,trsf,Matrix3r::Identity(),,"Transformation (rotation) from global to local coordinates. (the translation part is in :yref:`GenericSpheresContact.contactPoint`)"))
		,
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(L3Geom,GenericSpheresContact);
};
REGISTER_SERIALIZABLE(L3Geom);

/*
class L3Geom_Sphere_Sphere: public L3Geom{
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(L3Geom_Sphere_Sphere,L3Geom,"Local geometry of contact between two spherical particles with 3 DoFs. [experimental]",
		((

}
*/

class Ig2_Sphere_Sphere_L3Geom_Inc: public IGeomFunctor{
		virtual bool go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I);

	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_L3Geom_Inc,IGeomFunctor,"Incrementally compute :yref:`L3Geom` for contact of 2 spheres.\n\n.. note:: The initial value of *u[0]* (normal displacement) might be non-zero, with or without *distFactor*, since it is given purely by sphere's geometry. If you want to set \"equilibrium distance\", do it in the contact law as explained in :yref:`L3Geom.u0`.",
		((bool,noRatch,true,,"See :yref:`ScGeom.avoidGranularRatcheting`."))
		((Real,distFactor,1,,"Create interaction if spheres are not futher than distFactor*(r1+r2)."))
	);
	FUNCTOR2D(Sphere,Sphere);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_L3Geom_Inc);
