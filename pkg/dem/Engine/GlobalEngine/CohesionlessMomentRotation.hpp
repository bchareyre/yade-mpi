/* CWBoon@2010  booncw@hotmail.com */

#pragma once
#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

/* Contact law have been verified with Plassiard et al. (2009) : A spherical discrete element model: calibration procedure and incremental response */
/* Can be used in Triaxial Test, but significant changes has to be made */

/*    CHANGES THAT HAS TO BE DONE ON TRIAXIAL TEST */
/*
Ip2_MomentMat_MomentMat_MomentPhys and Law2_SCG_MomentPhys_CohesionlessMomentRotation have to be added. Since it uses ScGeom, it uses boxes rather than facets.  Spheres and boxes have to be changed to MomentMat rather than FrictMat

  INPUT FOR Ip2_MomentMat_MomentMat_MomentPhys: 

1.  If boolean userInputStiffness=true & useAlphaBeta=false, users can input Knormal, Kshear and Krotate directly.  Then, kn,ks and kr will be equal to these values, rather than calculated E and v.

2.  If boolean userInputStiffness=true & useAlphaBeta=true, users input Knormal, Alpha and Beta.  Then ks and kr are calculated from alpha & beta respectively

3.  If both are false, it calculates kn and ks are calculated from E and v, whilst kr = 0.

  
  INPUT FOR MomentMat:

1.  Users can input eta (constant for plastic moment) to Spheres and Boxes. For more complicated cases, users can modify TriaxialStressController to use different eta values during isotropic compaction.

  CONTACT LAW:
  The contribution of stiffnesses are scaled according to the radius of the particle, as implemented in Plassiard et al. (2009)
*/




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
		Vector3r prevNormal, shear; 
		Real cumulativeRotation;
		Quaternionr	initialOrientation1,initialOrientation2;
		Real		kr; // rolling stiffness
		Vector3r	moment_twist,moment_bending;
		MomentPhys();
	
	virtual ~MomentPhys();

	REGISTER_ATTRIBUTES(NormShearPhys,(tanFrictionAngle) (frictionAngle) (prevNormal) (initialOrientation1) (initialOrientation2) (kr) (Eta)(moment_twist) (moment_bending) (cumulativeRotation)(shear));
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

