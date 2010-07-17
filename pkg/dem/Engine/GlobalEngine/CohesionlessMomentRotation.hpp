/* CWBoon@2010  booncw@hotmail.com */

#pragma once
#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<set>
#include<boost/tuple/tuple.hpp>


class Law2_SCG_MomentPhys_CohesionlessMomentRotation: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I);
	FUNCTOR2D(ScGeom,MomentPhys);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_SCG_MomentPhys_CohesionlessMomentRotation,LawFunctor,"Contact law based on Plassiard et al. (2009) : A spherical discrete element model: calibration procedure and incremental response. The functionality has been verified with results in the paper.\n\nThe contribution of stiffnesses are scaled according to the radius of the particle, as implemented in that paper.\n\nSee also associated classes :yref:`MomentMat`, :yref:`Ip2_MomentMat_MomentMat_MomentPhys`, :yref:`MomentPhys`.\n\n.. note::\n\tThis constitutive law can be used with triaxial test, but the following significant changes in code have to be made: :yref:`Ip2_MomentMat_MomentMat_MomentPhys` and :yref:`Law2_SCG_MomentPhys_CohesionlessMomentRotation` have to be added. Since it uses :yref:`ScGeom`, it uses :yref:`boxes<Box>` rather than :yref:`facets<Facet>`. :yref:`Spheres<Sphere>` and :yref:`boxes<Box>` have to be changed to :yref:`MomentMat` rather than :yref:`FrictMat`.",
		((bool,preventGranularRatcheting,false,"??"))
	);
	DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_SCG_MomentPhys_CohesionlessMomentRotation);


class Ip2_MomentMat_MomentMat_MomentPhys: public InteractionPhysicsFunctor{
	public:
		virtual ~Ip2_MomentMat_MomentMat_MomentPhys();
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		FUNCTOR2D(MomentMat,MomentMat);
		DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_MomentMat_MomentMat_MomentPhys,InteractionPhysicsFunctor,"Create :yref:`MomentPhys` from 2 instances of :yref:`MomentMat`.\n\n1.  If boolean userInputStiffness=true & useAlphaBeta=false, users can input Knormal, Kshear and Krotate directly.  Then, kn,ks and kr will be equal to these values, rather than calculated E and v.\n\n2.  If boolean userInputStiffness=true & useAlphaBeta=true, users input Knormal, Alpha and Beta.  Then ks and kr are calculated from alpha & beta respectively.\n\n3.  If both are false, it calculates kn and ks are calculated from E and v, whilst kr = 0.",
		((bool,userInputStiffness,false,"for users to choose whether to input stiffness directly or use ratios to calculate Ks/Kn"))
		((bool,useAlphaBeta,false,"for users to choose whether to input stiffness directly or use ratios to calculate Ks/Kn"))
		((Real,Knormal,0,"Allows user to input stiffness properties from triaxial test. These will be passed to :yref:`MomentPhys` or :yref:`NormShearPhys`"))
		((Real,Kshear,0,"Allows user to input stiffness properties from triaxial test. These will be passed to :yref:`MomentPhys` or :yref:`NormShearPhys`"))
		((Real,Krotate,0,"Allows user to input stiffness properties from triaxial test. These will be passed to :yref:`MomentPhys` or :yref:`NormShearPhys`"))
		((Real,Alpha,0,"Ratio of Ks/Kn"))
		((Real,Beta,0,"Ratio to calculate Kr"))
	);
};
REGISTER_SERIALIZABLE(Ip2_MomentMat_MomentMat_MomentPhys);


class MomentPhys: public NormShearPhys {
	public:
		virtual ~MomentPhys();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(MomentPhys,NormShearPhys,"Physical interaction properties for use with :yref:`Law2_SCG_MomentPhys_CohesionlessMomentRotation`, created by :yref:`Ip2_MomentMat_MomentMat_MomentPhys`.",
		((Real,frictionAngle,0,"Friction angle [rad]"))
		((Real,tanFrictionAngle,0,"Tangent of friction angle"))
		((Real,Eta,0,"??"))
		((Quaternionr,initialOrientation1,Quaternionr::Identity(),"??"))
		((Quaternionr,initialOrientation2,Quaternionr::Identity(),"??"))
		((Vector3r,prevNormal,Vector3r::Zero(),"Normal in the previous step."))
		((Real,kr,0,"rolling stiffness"))
		((Vector3r,moment_twist,Vector3r::Zero(),"??"))
		((Vector3r,moment_bending,Vector3r::Zero(),"??"))
		((Real,cumulativeRotation,0,"??"))
		((Vector3r,shear,Vector3r::Zero(),"??")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(MomentPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(MomentPhys);

/** This class holds information associated with each body */
class MomentMat: public FrictMat {
	public:
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(MomentMat,FrictMat,"Material for constitutive law of (Plassiard & al., 2009); see :yref:`Law2_SCG_MomentPhys_CohesionlessMomentRotation` for details.\n\nUsers can input eta (constant for plastic moment) to Spheres and Boxes. For more complicated cases, users can modify TriaxialStressController to use different eta values during isotropic compaction.",
		((Real,eta,0,"(has to be stored in this class and not by ContactLaw, because users may want to change its values before/after isotropic compaction.)")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(MomentMat,FrictMat);
};
REGISTER_SERIALIZABLE(MomentMat);

