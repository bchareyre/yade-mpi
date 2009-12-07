// CWBoon@2009  booncw@hotmail.com //

#pragma once
#include<yade/pkg-common/ElasticMat.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

//! This is the simplest law with Kn, Ks and Coulomb.  It is a duplication of Law2_Dem3Dof_Elastic_Elastic, but it is cleaner.
//! It also shows clearly to a beginner on how to write a Constitutive Law.  In a sense, it is similar to RockPM

class Law2_Dem3Dof_CSPhys_CundallStrack: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
		FUNCTOR2D(Dem3DofGeom,CSPhys);
		REGISTER_CLASS_AND_BASE(Law2_Dem3Dof_CSPhys_CundallStrack,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_Dem3Dof_CSPhys_CundallStrack);

class Ip2_BMP_BMP_CSPhys: public InteractionPhysicsFunctor{
	public:

		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,);
		FUNCTOR2D(GranularMat,GranularMat);
		REGISTER_CLASS_AND_BASE(Ip2_BMP_BMP_CSPhys,InteractionPhysicsFunctor);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_BMP_BMP_CSPhys);


class CSPhys: public NormalShearInteraction {
	private:
	public:
		Real frictionAngle, tanFrictionAngle; 
		CSPhys(): NormalShearInteraction(), frictionAngle(0),tanFrictionAngle(0){ createIndex(); }
	virtual ~CSPhys();

	REGISTER_ATTRIBUTES(NormalShearInteraction,(tanFrictionAngle) (frictionAngle));
	REGISTER_CLASS_AND_BASE(CSPhys,NormalShearInteraction);
	REGISTER_CLASS_INDEX(CSPhys,NormalShearInteraction);
};
REGISTER_SERIALIZABLE(CSPhys);
