// 2010 © Chiara Modenese <c.modenese@gmail.com> 
// 
/*
=== HIGH LEVEL OVERVIEW OF Mindlin ===

Mindlin is a set of classes to include the Hertz-Mindlin formulation for the contact stiffnesses.

*/

#pragma once

#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-common/Dispatching.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>


#include <set>
#include <boost/tuple/tuple.hpp>
#include<yade/lib-base/openmp-accu.hpp>


/******************** MindlinPhys *********************************/
class MindlinPhys: public FrictPhys{
	public:
	virtual ~MindlinPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(MindlinPhys,FrictPhys,"Representation of an interaction of the Hertz-Mindlin type.",
			((Real,kno,0.0,,"Constant value in the formulation of the normal stiffness"))
			((Real,kso,0.0,,"Constant value in the formulation of the tangential stiffness"))
			((Vector3r,normalViscous,Vector3r::Zero(),,"Normal viscous component"))
			((Vector3r,shearViscous,Vector3r::Zero(),,"Shear viscous component"))
			((Vector3r,shearElastic,Vector3r::Zero(),,"Total elastic shear force"))
			((Vector3r,usElastic,Vector3r::Zero(),,"Total elastic shear displacement (only elastic part)"))
			((Vector3r,usTotal,Vector3r::Zero(),,"Total elastic shear displacement (elastic+plastic part)"))
			((Real,radius,NaN,,"Contact radius (only computed with :yref:`Law2_ScGeom_MindlinPhys_Mindlin::calcEnergy`)"))

			//((Real,gamma,0.0,"Surface energy parameter [J/m^2] per each unit contact surface, to derive DMT formulation from HM"))
			((Real,adhesionForce,0.0,,"Force of adhesion as predicted by DMT"))
			((bool,isAdhesive,false,,"bool to identify if the contact is adhesive, that is to say if the contact force is attractive"))

			//((Real,shearEnergy,0.0,,"Shear elastic potential energy"))
			//((Real,frictionDissipation,0.0,,"Energy dissipation due to sliding"))
			//((Real,normDampDissip,0.0,,"Energy dissipation due to sliding"))
			//((Real,shearDampDissip,0.0,,"Energy dissipation due to sliding"))
			,
			createIndex());
	REGISTER_CLASS_INDEX(MindlinPhys,FrictPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(MindlinPhys);


/******************** Ip2_FrictMat_FrictMat_MindlinPhys *******/
class Ip2_FrictMat_FrictMat_MindlinPhys: public InteractionPhysicsFunctor{
	public :
	virtual void go(const shared_ptr<Material>& b1,	const shared_ptr<Material>& b2,	const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_FrictMat_FrictMat_MindlinPhys,InteractionPhysicsFunctor,"Calculate some physical parameters needed to obtain the normal and shear stiffnesses according to the Hertz-Mindlin's formulation (as implemented in PFC).",
			((Real,gamma,0.0,,"Surface energy parameter [J/m^2] per each unit contact surface, to derive DMT formulation from HM"))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_MindlinPhys);

class Law2_ScGeom_MindlinPhys_MindlinDeresiewitz: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*);
		FUNCTOR2D(ScGeom,MindlinPhys);
		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_MindlinPhys_MindlinDeresiewitz,LawFunctor,
			"Hertz-Mindlin contact law with partial slip solution, as described in [Thornton1991]_.",
		);
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MindlinPhys_MindlinDeresiewitz);

class Law2_ScGeom_MindlinPhys_HertzWithLinearShear: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*);
		FUNCTOR2D(ScGeom,MindlinPhys);
		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_MindlinPhys_HertzWithLinearShear,LawFunctor,
			"Constitutive law for the Hertz formulation (using :yref:`MindlinPhys.kno`) and linear beahvior in shear (using :yref:`MindlinPhys.kso` for stiffness and :yref:`FrictPhys.tangensOfFrictionAngle`). \n\n.. note:: No viscosity or damping. If you need those, look at  :yref:`Law2_ScGeom_MindlinPhys_Mindlin`, which also includes non-linear Mindlin shear.",
				((int,nonLin,0,,"Shear force nonlinearity (the value determines how many features of the non-linearity are taken in account). 1: ks as in HM 2: shearElastic increment computed as in HM 3. granular ratcheting disabled."))
		);
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MindlinPhys_HertzWithLinearShear);

/******************** Law2_ScGeom_MindlinPhys_Mindlin *********/
class Law2_ScGeom_MindlinPhys_Mindlin: public LawFunctor{
	public:
		static Real Real0;
		OpenMPAccumulator<Real,&Law2_ScGeom_MindlinPhys_Mindlin::Real0> frictionDissipation;
		OpenMPAccumulator<Real,&Law2_ScGeom_MindlinPhys_Mindlin::Real0> shearEnergy;
		OpenMPAccumulator<Real,&Law2_ScGeom_MindlinPhys_Mindlin::Real0> normDampDissip;
		OpenMPAccumulator<Real,&Law2_ScGeom_MindlinPhys_Mindlin::Real0> shearDampDissip;

		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I);
		Real normElastEnergy();
		Real getfrictionDissipation();
		Real getshearEnergy();
		Real getnormDampDissip();
		Real getshearDampDissip();
		Real contactsAdhesive();


		FUNCTOR2D(ScGeom,MindlinPhys);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_MindlinPhys_Mindlin,LawFunctor,"Constitutive law for the Hertz-Mindlin formulation. It includes non linear elasticity in the normal direction as predicted by Hertz for two non-conforming elastic contact bodies. In the shear direction, instead, it reseambles the simplified case without slip discussed in Mindlin's paper, where a linear relationship between shear force and tangential displacement is provided. Finally, the Mohr-Coulomb criterion is employed to established the maximum friction force which can be developed at the contact. Moreover, it is also possible to include the effect of linear viscous damping through the definition of the parameters $\\beta_{n}$ and $\\beta_{s}$.",
			((bool,preventGranularRatcheting,true,,"bool to avoid granular ratcheting"))
			((bool,includeAdhesion,false,,"bool to include the adhesion force following the DMT formulation. If true, also the normal elastic energy takes into account the adhesion effect."))
			((bool,useDamping,false,,"bool to include contact damping"))
			((bool,calcEnergy,false,,"bool to calculate energy terms (shear potential energy, dissipation of energy due to friction and dissipation of energy due to normal and tangential damping)"))
			((Real,betan,0.0,,"Fraction of the viscous damping coefficient (normal direction) equal to $\\frac{c_{n}}{C_{n,crit}}$."))
			((Real,betas,0.0,,"Fraction of the viscous damping coefficient (shear direction) equal to $\\frac{c_{s}}{C_{s,crit}}$."))
			// FIXME: all the energy attributes should be openMPAccumulator
			//((Real,shearEnergy,0.0,"Shear elastic potential energy"))
			//((Real,frictionDissipation,0.0,"Energy dissipation due to sliding"))
			//((Real,normDampDissip,0.0,"Energy dissipation due to sliding"))
			//((Real,shearDampDissip,0.0,"Energy dissipation due to sliding"))
			((Real,cn,0.0,Attr::readonly,"Damping normal coefficient"))
			((Real,cs,0.0,Attr::readonly,"Damping tangetial coefficient"))
			,
			/* ctor */
			,
			.def("contactsAdhesive",&Law2_ScGeom_MindlinPhys_Mindlin::contactsAdhesive,"Compute total number of adhesive contacts.")
			.def("normElastEnergy",&Law2_ScGeom_MindlinPhys_Mindlin::normElastEnergy,"Compute normal elastic potential energy. It handle the DMT formulation if :yref:`Law2_ScGeom_MindlinPhys_Mindlin::includeAdhesion` is set to true.")
			.def("frictionDissipation",&Law2_ScGeom_MindlinPhys_Mindlin::getfrictionDissipation,"Energy dissipated by frictional behavior.")
			.def("shearEnergy",&Law2_ScGeom_MindlinPhys_Mindlin::getshearEnergy,"Shear elastic potential energy.")
			.def("normDampDissip",&Law2_ScGeom_MindlinPhys_Mindlin::getnormDampDissip,"Energy dissipated by normal damping.")
			.def("shearDampDissip",&Law2_ScGeom_MindlinPhys_Mindlin::getshearDampDissip,"Energy dissipated by tangential damping.")
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MindlinPhys_Mindlin);









