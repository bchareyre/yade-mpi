// 2010 © Chiara Modenese <c.modenese@gmail.com> 
// 
/*
=== HIGH LEVEL OVERVIEW OF Mindlin ===

Mindlin is a set of classes to include the Hertz-Mindlin formulation for the contact stiffnesses.

*/

#pragma once

#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/LawFunctor.hpp>


/******************** MindlinPhys *********************************/
class MindlinPhys: public FrictPhys{
	public:
	virtual ~MindlinPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(MindlinPhys,FrictPhys,"Representation of an interaction of the Mindlin type.",
			((Real,kno,0.0,"Constant value in the formulation of the normal stiffness"))
			((Real,kso,0.0,"Constant value in the formulation of the tangential stiffness")),
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
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
	FUNCTOR2D(ScGeom,MindlinPhys);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_MindlinPhys_Mindlin,LawFunctor,"Constitutive law for the Mindlin's formulation.",
			((bool,preventGranularRatcheting,true,"bool to avoid granular ratcheting"))

	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MindlinPhys_Mindlin);









