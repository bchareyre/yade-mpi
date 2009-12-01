// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
// This file contains a set of classes for modelling of viscoelastic
// particles. 

#pragma once

#include<yade/core/Material.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/ConstitutiveLaw.hpp>

/* Simple viscoelastic model */
 
/// Material
/// Note: Shop::getViscoelasticFromSpheresInteraction can get kn,cn,ks,cs from a analytical solution of a pair spheres interaction problem.
class SimpleViscoelasticMat : public Material {	
	public :
	    /// Normal elasticity
	    Real kn; 
	    /// Normal viscosity
	    Real cn; 
	    /// Shear elasticity
	    Real ks; 
	    /// Shear viscosity
	    Real cs; 
	    /// Friction angle
	    Real frictionAngle; 
		SimpleViscoelasticMat(){ createIndex(); }
		virtual ~SimpleViscoelasticMat();
	REGISTER_ATTRIBUTES(Material,(kn)(ks)(cn)(cs)(frictionAngle));
	REGISTER_CLASS_AND_BASE(SimpleViscoelasticMat,Material);
	REGISTER_CLASS_INDEX(SimpleViscoelasticMat,Material);
};
REGISTER_SERIALIZABLE(SimpleViscoelasticMat);

/// Interaction physics
class SimpleViscoelasticPhys : public ElasticContactInteraction {
	public :
	    /// Normal viscous 
	    Real cn;
	    /// Shear viscous 
	    Real cs;
		SimpleViscoelasticPhys(){ createIndex(); }
		virtual ~SimpleViscoelasticPhys();
	protected :
	REGISTER_ATTRIBUTES(ElasticContactInteraction,(cn)(cs));
	REGISTER_CLASS_AND_BASE(SimpleViscoelasticPhys,ElasticContactInteraction);
	REGISTER_CLASS_INDEX(SimpleViscoelasticPhys,ElasticContactInteraction);
};
REGISTER_SERIALIZABLE(SimpleViscoelasticPhys);

/// Convert material to interaction physics.
// Uses the rule of consecutively connection.
class Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys: public InteractionPhysicsFunctor {
	public :
		virtual void go(const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/* */)
	FUNCTOR2D(SimpleViscoelasticMat,SimpleViscoelasticMat);
	REGISTER_CLASS_AND_BASE(Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys, InteractionPhysicsFunctor);

};
REGISTER_SERIALIZABLE(Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys);

/// Constitutive law
/// This class provides linear viscoelastic contact model
class Law2_Spheres_Viscoelastic_SimpleViscoelastic: public ConstitutiveLaw {
	public :
		virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, MetaBody*);
		FUNCTOR2D(SpheresContactGeometry,SimpleViscoelasticPhys);
		REGISTER_CLASS_AND_BASE(Law2_Spheres_Viscoelastic_SimpleViscoelastic,ConstitutiveLaw);
		REGISTER_ATTRIBUTES(ConstitutiveLaw,/* */);
};
REGISTER_SERIALIZABLE(Law2_Spheres_Viscoelastic_SimpleViscoelastic);

