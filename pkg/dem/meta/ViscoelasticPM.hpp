// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
// This file contains a set of classes for modelling of viscoelastic
// particles. 

#pragma once

#include<yade/core/Material.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

/* Simple viscoelastic model */
 
/// Material
/// Note: Shop::getViscoelasticFromSpheresInteraction can get kn,cn,ks,cs from a analytical solution of a pair spheres interaction problem.
class SimpleViscoelasticMat : public Material {	
	public:
		virtual ~SimpleViscoelasticMat();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(SimpleViscoelasticMat,Material,"Material for simple viscoelastic model of contact.\n\n.. note::\n\t ``Shop::getViscoelasticFromSpheresInteraction`` (and :yref:`yade.utils.getViscoelasticFromSpheresInteraction` in python) compute :yref:`kn<SimpleViscoelasticMat::kn>`, :yref:`cn<SimpleViscoelasticMat::cn>`,  :yref:`ks<SimpleViscoelasticMat::ks>`,  :yref:`cs<SimpleViscoelasticMat::cs>` from analytical solution of a pair spheres interaction problem.",
		((Real,kn,NaN,"Normal elastic stiffness"))
		((Real,cn,NaN,"Normal viscous constant"))
		((Real,ks,NaN,"Shear elastic stiffness"))
		((Real,cs,NaN,"Shear viscous constant"))
		((Real,frictionAngle,NaN,"Friction angle [rad]")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(SimpleViscoelasticMat,Material);
};
REGISTER_SERIALIZABLE(SimpleViscoelasticMat);

/// Interaction physics
class SimpleViscoelasticPhys : public FrictPhys {
	public :
	    /// Normal viscous 
	    Real cn;
	    /// Shear viscous 
	    Real cs;
		SimpleViscoelasticPhys(){ createIndex(); }
		virtual ~SimpleViscoelasticPhys();
	protected :
	REGISTER_ATTRIBUTES(FrictPhys,(cn)(cs));
	REGISTER_CLASS_AND_BASE(SimpleViscoelasticPhys,FrictPhys);
	REGISTER_CLASS_INDEX(SimpleViscoelasticPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(SimpleViscoelasticPhys);

/// Convert material to interaction physics.
// Uses the rule of consecutively connection.
class Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys: public InteractionPhysicsFunctor {
	public :
		virtual void go(const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
	YADE_CLASS_BASE_DOC(Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys,InteractionPhysicsFunctor,"Convert 2 instances of :yref:`SimpleViscoelasticMat` to :yref:`SimpleViscoelasticPhys` using the rule of consecutive connection.");
	FUNCTOR2D(SimpleViscoelasticMat,SimpleViscoelasticMat);

};
REGISTER_SERIALIZABLE(Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys);

/// Constitutive law
/// This class provides linear viscoelastic contact model
class Law2_Spheres_Viscoelastic_SimpleViscoelastic: public LawFunctor {
	public :
		virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, Scene*);
	FUNCTOR2D(ScGeom,SimpleViscoelasticPhys);
	YADE_CLASS_BASE_DOC(Law2_Spheres_Viscoelastic_SimpleViscoelastic,LawFunctor,"Linear viscoelastic model operating on :yref:`ScGeom` and :yref:`SimpleViscoelasticPhys`.");
};
REGISTER_SERIALIZABLE(Law2_Spheres_Viscoelastic_SimpleViscoelastic);

