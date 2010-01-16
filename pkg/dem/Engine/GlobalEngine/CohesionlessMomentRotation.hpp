#pragma once
#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>


class Law2_SCG_MomentPhys_CohesionlessMomentRotation: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
		bool preventGranularRatcheting;
		Law2_SCG_MomentPhys_CohesionlessMomentRotation():preventGranularRatcheting(false){};
		FUNCTOR2D(ScGeom,MomentPhys);
		REGISTER_CLASS_AND_BASE(Law2_SCG_MomentPhys_CohesionlessMomentRotation,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_SCG_MomentPhys_CohesionlessMomentRotation);


class Ip2_MomentMat_MomentMat_MomentPhys: public InteractionPhysicsFunctor{
	public:
		bool userInputStiffness, useAlphaBeta; //for users to choose whether to input stiffness directly or use ratios to calculate Ks/Kn
		Real Knormal, Kshear, Krotate; //allows user to input stiffness properties from triaxial test.  These will be passed to MomentPhys or NormShearPhys
		Real Alpha, Beta; //Alpha is a ratio of Ks/Kn, Beta is a ratio to calculate Kr
		Ip2_MomentMat_MomentMat_MomentPhys();
		virtual ~Ip2_MomentMat_MomentMat_MomentPhys();
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,(userInputStiffness)(useAlphaBeta)(Knormal)(Kshear)(Krotate)(Alpha)(Beta));
		FUNCTOR2D(MomentMat,MomentMat);
		REGISTER_CLASS_AND_BASE(Ip2_MomentMat_MomentMat_MomentPhys,InteractionPhysicsFunctor);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_MomentMat_MomentMat_MomentPhys);


class MomentPhys: public NormShearPhys {
	private:
	public:
		Real frictionAngle, tanFrictionAngle, Eta;  
		Vector3r prevNormal;
		Vector3r shear;
		Real cumulativeRotation;
		Quaternionr	initialOrientation1,initialOrientation2;
		Real		kr; // rolling stiffness
		Vector3r	moment_twist,moment_bending;
		MomentPhys();
	
	virtual ~MomentPhys();

	REGISTER_ATTRIBUTES(NormShearPhys,(tanFrictionAngle) (frictionAngle) (prevNormal) (initialOrientation1) (initialOrientation2) (kr) (Eta)(moment_twist) (moment_bending) (shear)(cumulativeRotation));
	REGISTER_CLASS_AND_BASE(MomentPhys,NormShearPhys);
	REGISTER_CLASS_INDEX(MomentPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(MomentPhys);

/** This class holds information associated with each body */
class MomentMat: public FrictMat {
	public:
		Real eta; //It has to be stored in this class and not by ContactLaw, because users may want to change its values before/after isotropic compaction.
		MomentMat(): eta(0) {createIndex();};
		REGISTER_ATTRIBUTES(FrictMat, (eta));
		REGISTER_CLASS_AND_BASE(MomentMat,FrictMat);
		REGISTER_CLASS_INDEX(MomentMat,FrictMat);
};
REGISTER_SERIALIZABLE(MomentMat);

