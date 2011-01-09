// CWBoon@2009  booncw@hotmail.com //

#pragma once
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/dem/DemXDofGeom.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>


class CSPhys: public NormShearPhys {
	public:
	virtual ~CSPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CSPhys,NormShearPhys,"Physical properties for :yref:`Cundall&Strack constitutive law<Law2_Dem3Dof_CSPhys_CundallStrack>`, created by :yref:`Ip2_2xFrictMat_CSPhys`.",
		((Real,frictionAngle,NaN,,"Friction angle of the interaction. |ycomp|"))
		((Real,tanFrictionAngle,NaN,,"Precomputed tangent of :yref:`CSPhys::frictionAngle`. |ycomp|")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(CSPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(CSPhys);

//! This is the simplest law with Kn, Ks and Coulomb.  It is a duplication of Law2_Dem3DofGeom_FrictPhys_CundallStrack, but it is cleaner.
//! It also shows clearly to a beginner on how to write a Constitutive Law.  In a sense, it is similar to RockPM

class Law2_Dem3Dof_CSPhys_CundallStrack: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(Dem3DofGeom,CSPhys);
		YADE_CLASS_BASE_DOC(Law2_Dem3Dof_CSPhys_CundallStrack,LawFunctor,"Basic constitutive law published originally by Cundall&Strack; it has normal and shear stiffnesses (Kn, Kn) and dry Coulomb friction. Operates on associated :yref:`Dem3DofGeom` and :yref:`CSPhys` instances.");
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_Dem3Dof_CSPhys_CundallStrack);

class Ip2_2xFrictMat_CSPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		FUNCTOR2D(FrictMat,FrictMat);
		YADE_CLASS_BASE_DOC(Ip2_2xFrictMat_CSPhys,IPhysFunctor,"Functor creating :yref:`CSPhys` from  two :yref:`FrictMat`. See :yref:`Law2_Dem3Dof_CSPhys_CundallStrack` for details.");
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_2xFrictMat_CSPhys);


