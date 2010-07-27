// 2010 © Chiara Modenese <c.modenese@gmail.com> 
// 
/*
=== HIGH LEVEL OVERVIEW OF Mindlin ===

Mindlin is a set of classes to include the Hertz-Mindlin formulation for the contact stiffnesses.

*/

#pragma once

#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/Dispatching.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>


/******************** MindlinPhys *********************************/
class MindlinPhys: public FrictPhys{
	public:
	virtual ~MindlinPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(MindlinPhys,FrictPhys,"Representation of an interaction of the Hertz-Mindlin type.",
			((Real,kno,0.0,"Constant value in the formulation of the normal stiffness"))
			((Real,kso,0.0,"Constant value in the formulation of the tangential stiffness"))
			((Vector3r,normalViscous,Vector3r::Zero(),"Normal viscous component"))
			((Vector3r,shearViscous,Vector3r::Zero(),"Normal viscous component"))
			((Vector3r,shearElastic,Vector3r::Zero(),"Total elastic shear force"))
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
	YADE_CLASS_BASE_DOC(Ip2_FrictMat_FrictMat_MindlinPhys,InteractionPhysicsFunctor,"Calculate some physical parameters needed to obtain the normal and shear stiffnesses according to the Hertz-Mindlin's formulation (as implemented in PFC).");
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_MindlinPhys);


/******************** Law2_ScGeom_MindlinPhys_Mindlin *********/
class Law2_ScGeom_MindlinPhys_Mindlin: public LawFunctor{
	public:
		Real cn, cs;
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I);
		FUNCTOR2D(ScGeom,MindlinPhys);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_MindlinPhys_Mindlin,LawFunctor,"Constitutive law for the Hertz-Mindlin formulation. It includes non linear elasticity in the normal direction as predicted by Hertz for two non-conforming elastic contact bodies. In the shear direction, instead, it reseambles the simplified case without slip discussed in Mindlin's paper, where a linear relationship between shear force and tangential displacement is provided. Finally, the Mohr-Coulomb criterion is employed to established the maximum friction force which can be developed at the contact. Moreover, it is also possible to include the effect of contact damping through the definition of the parameters $\\beta_{n}$ and $\\beta_{s}$.",
			((bool,preventGranularRatcheting,false,"bool to avoid granular ratcheting"))
			((bool,useDamping,false,"bool to include contact damping"))
			((Real,betan,0.0,"Fraction of the viscous damping coefficient (normal direction) equal to $\\frac{c_{n}}{C_{n,crit}}$."))
			((Real,betas,0.0,"Fraction of the viscous damping coefficient (shear direction) equal to $\\frac{c_{s}}{C_{s,crit}}$."))
			,
			cn = 0.0; cs = 0.0;
			,
			.def_readonly("cn",&Law2_ScGeom_MindlinPhys_Mindlin::cn,"Damping normal coefficient.")
			.def_readonly("cs",&Law2_ScGeom_MindlinPhys_Mindlin::cs,"Damping tangential coefficient.")
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MindlinPhys_Mindlin);









