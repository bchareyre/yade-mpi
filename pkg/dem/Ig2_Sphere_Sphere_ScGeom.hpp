// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2007 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/pkg/common/Dispatching.hpp>

class Ig2_Sphere_Sphere_ScGeom: public IGeomFunctor{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#ifdef YADE_DEVIRT_FUNCTORS
		void* getStaticFuncPtr(){ return (void*)&Ig2_Sphere_Sphere_ScGeom::goStatic; }
		static bool goStatic(IGeomFunctor* self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#endif
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_ScGeom,IGeomFunctor,"Create/update a :yref:`ScGeom` instance representing intersection of two :yref:`Spheres<Sphere>`.",
		((Real,interactionDetectionFactor,1,,"Enlarge both radii by this factor (if >1), to permit creation of distant interactions.\n\nInteractionGeometry will be computed when interactionDetectionFactor*(rad1+rad2) > distance.\n\n.. note::\n\t This parameter is functionally coupled with :yref:`Bo1_Sphere_Aabb::aabbEnlargeFactor`, which will create larger bounding boxes and should be of the same value.\n\n.. warning::\n\tFunctionally equal class :yref:`Ig2_Sphere_Sphere_Dem3DofGeom` (which creates :yref:`Dem3DofGeom` rather than :yref:`ScGeom`) calls this parameter :yref:`distFactor<Ig2_Sphere_Sphere_Dem3DofGeom::distFactor>`, but its semantics is *different* in some aspects."))
		((bool,avoidGranularRatcheting,true,,"Granular ratcheting is mentioned in [GarciaRojo2004]_, [Alonso2004]_, [McNamara2008]_.\n\n"
				"Unfortunately, published papers tend to focus on the \"good\" ratcheting, i.e. irreversible deformations due to the intrinsic nature of frictional granular materials, while a 2004 talk of McNamara in Paris clearly mentioned a possible \"bad\" ratcheting, purely linked with the formulation of the contact laws in what he called \"molecular dynamics\" (i.e. Cundall's model, as opposed to \"contact dynamics\" from Moreau and Jean).\n\n"
				"The bad ratcheting is best understood considering this small elastic cycle at a contact between two grains: assuming b1 is fixed, impose this displacement to b2:\n\n"
				"#. translation *dx* in the normal direction\n"
			 	"#. rotation *a*\n"
			 	"#. translation *-dx* (back to the initial position)\n"
			 	"#. rotation *-a* (back to the initial orientation)\n\n\n"
				"If the branch vector used to define the relative shear in rotation×branch is not constant (typically if it is defined from the vector center→contactPoint), then the shear displacement at the end of this cycle is not zero: rotations *a* and *-a* are multiplied by branches of different lengths.\n\n"
				"It results in a finite contact force at the end of the cycle even though the positions and orientations are unchanged, in total contradiction with the elastic nature of the problem. It could also be seen as an *inconsistent energy creation or loss*. Given that DEM simulations tend to generate oscillations around equilibrium (damped mass-spring), it can have a significant impact on the evolution of the packings, resulting for instance in slow creep in iterations under constant load.\n\n"
				"The solution to avoid that is quite simple in the case of linear-elastic laws: use a constant branch vector, which is what this functor does by default."
		))
	);
	FUNCTOR2D(Sphere,Sphere);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_ScGeom);

class Ig2_Sphere_Sphere_ScGeom6D: public Ig2_Sphere_Sphere_ScGeom{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#ifdef YADE_DEVIRT_FUNCTORS
		void* getStaticFuncPtr(){ return (void*)&Ig2_Sphere_Sphere_ScGeom6D::goStatic; }
		static bool goStatic(IGeomFunctor* self, const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& se32, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
	#endif
	YADE_CLASS_BASE_DOC_ATTRS(Ig2_Sphere_Sphere_ScGeom6D,Ig2_Sphere_Sphere_ScGeom,"Create/update a :yref:`ScGeom6D` instance representing intersection of two :yref:`Spheres<Sphere>`.",
		((bool,updateRotations,true,,"Precompute relative rotations. Turning this false can speed up simulations when rotations are not needed in constitutive laws (e.g. when spheres are compressed without cohesion and moment in early stage of a triaxial test), but is not foolproof. Change this value only if you know what you are doing."))
		((bool,creep,false,,"Substract rotational creep from relative rotation. The rotational creep :yref:`ScGeom6D::twistCreep` is a quaternion and has to be updated inside a constitutive law, see for instance :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment`."
		))
	);
	FUNCTOR2D(Sphere,Sphere);
	// needed for the dispatcher, even if it is symmetric
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Sphere);
};
REGISTER_SERIALIZABLE(Ig2_Sphere_Sphere_ScGeom6D);

