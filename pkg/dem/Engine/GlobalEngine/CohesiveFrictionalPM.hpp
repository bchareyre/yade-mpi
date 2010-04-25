/* lucScholtes2010 */

#pragma once

#include<yade/pkg-common/ElastMat.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>

/*
=== OVERVIEW OF CohesiveFrictionalPM ===

CohesiveFrictional Particle Model (CohesiveFrictionalPM, CFpm) is a set of classes for modelling mechanical behavior of cohesive frictional material. It is a quite general contact model starting from the basic frictional model with the possible addition of a moment between particles, a tensile strength and a cohesion.

Features of the interaction law:

1.  If useAlphaBeta=False, Kn, Ks are calculated from the harmonic average of the "macro" material parameters young (E) and poisson (V) as defined in SimpleElasticRelationships.cpp and Kr=0.

2.  If useAlphaBeta=True, users have to input Alpha=Ks/Kn, Beta=Kr/(Ks*meanRadius^2), eta=MtPlastic/(meanRadius*Fn) as defined in Plassiard et al. (Granular Matter, 2009) A spherical discrete element model.

3.  Users can input a tensile strength ( FnMax = tensileStrength*pi*Rmin^2 ) and a cohesion ( FsMax = cohesion*pi*Rmin^2 )

  Remark: - This contact law works well in the case of sphere/sphere and sphere/box interaction as it uses ScGeom to compute the interaction geometry (suitable for the triaxialTest) 
	  - It has not been tested for sphere/facet or sphere/wall interactions and could be updated to be used by DemXDofGeom
*/

/** This class holds information associated with each body state*/
class CFpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS(CFpmState,State,"CFpm state information about each body.\n\nNone of that is used for computation (at least not now), only for post-processing.",
		((int,numBrokenCohesive,0,"Number of (cohesive) contacts that damaged completely"))
	);
};
REGISTER_SERIALIZABLE(CFpmState);

/** This class holds information associated with each body */
class CFpmMat: public FrictMat {
	public:
		virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new CFpmState); }
		virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<CFpmState*>(s); }
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CFpmMat,FrictMat,"cohesive frictional material, for use with other CFpm classes",
	  ((int,type,0,"Type of the particle. If particles of two different types interact, it will be with friction only (no cohesion).[-]")),
	  createIndex();
	);
	REGISTER_CLASS_INDEX(CFpmMat,FrictMat);
};
REGISTER_SERIALIZABLE(CFpmMat);

/** This class holds information associated with each interaction */
class CFpmPhys: public NormShearPhys {
	public:
		virtual ~CFpmPhys();
	
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(CFpmPhys,NormShearPhys,"Representation of a single interaction of the CFpm type, storage for relevant parameters",
		  ((Real,initD,0,"equilibrium distance for particles. Computed as the initial interparticular distance when bonded particle interact. initD=0 for non cohesive interactions."))
		  ((bool,isCohesive,false,"If false, particles interact in a frictional way. If true, particles are bonded regarding the given cohesion and tensileStrength."))
		  ((Real, frictionAngle,0,"defines Coulomb friction. [deg]"))
		  ((Real,tanFrictionAngle,0,"Tangent of frictionAngle. [-]"))
		  ((Real,FnMax,0,"Defines the maximum admissible normal force in traction FnMax=tensileStrength*crossSection, with crossSection=pi*Rmin^2. [Pa]"))
		  ((Real,FsMax,0,"Defines the maximum admissible tangential force in shear FsMax=cohesion*FnMax, with crossSection=pi*Rmin^2. [Pa]"))
		  ((Real,strengthSoftening,0,"Defines the softening when Dtensile is reached to avoid explosion. Typically, when D > Dtensile, Fn=FnMax - (kn/strengthSoftening)*(Dtensile-D). [-]"))
		  ((Real,cumulativeRotation,0,"Cumulated rotation... [-]"))
		  ((Real,kr,0,"Defines the stiffness to compute the resistive moment in rotation. [-]"))
		  ((Real,maxBend,0,"Defines the maximum admissible resistive moment in rotation Mtmax=maxBend*Fn, maxBend=eta*meanRadius. [m]"))
		  ((Vector3r,prevNormal,Vector3r::Zero(),"Normal to the contact at previous time step."))
		  ((Vector3r,moment_twist,Vector3r::Zero()," [N.m]"))
		  ((Vector3r,moment_bending,Vector3r::Zero()," [N.m]"))
		  ((Quaternionr,initialOrientation1,Quaternionr(1.0,0.0,0.0,0.0),"Used for moment computation."))
		  ((Quaternionr,initialOrientation2,Quaternionr(1.0,0.0,0.0,0.0),"Used for moment computation."))
		  ,
		  createIndex();
		  ,
		);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(CFpmPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(CFpmPhys);

/** 2d functor creating InteractionPhysics (Ip2) taking CFpmMat and CFpmMat of 2 bodies, returning type CFpmPhys */
class Ip2_CFpmMat_CFpmMat_CFpmPhys: public InteractionPhysicsFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		
		FUNCTOR2D(CFpmMat,CFpmMat);
		DECLARE_LOGGER;
		
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_CFpmMat_CFpmMat_CFpmPhys,InteractionPhysicsFunctor,"Converts 2 CFpmmat instances to CFpmPhys with corresponding parameters.",
		  ((int,cohesiveTresholdIteration,1,"Should new contacts be cohesive? They will before this iter, they won't afterward."))
		  ((bool,useAlphaBeta,false,"If true, stiffnesses are computed based on Alpha and Beta."))
		  ((Real,Alpha,0,"Defines the ratio ks/kn."))
		  ((Real,Beta,0,"Defines the ratio kr/(ks*meanRadius^2) to compute the resistive moment in rotation. [-]"))
		  ((Real,eta,0,"Defines the maximum admissible resistive moment in rotation MtMax=eta*meanRadius*Fn. [-]"))
		  ((Real,tensileStrength,0,"Defines the maximum admissible normal force in traction FnMax=tensileStrength*crossSection. [Pa]"))
		  ((Real,cohesion,0,"Defines the maximum admissible tangential force in shear FsMax=cohesion*crossSection. [Pa]"))
		  ((Real,strengthSoftening,0,"Defines the softening when Dtensile is reached to avoid explosion of the contact. Typically, when D > Dtensile, Fn=FnMax - (kn/strengthSoftening)*(Dtensile-D). [-]"))
		);
};
REGISTER_SERIALIZABLE(Ip2_CFpmMat_CFpmMat_CFpmPhys);


/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and CFpmPhys of 2 bodies, returning type CohesiveFrictionalPM */
class Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
		FUNCTOR2D(ScGeom,CFpmPhys);

	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM,LawFunctor,"Constitutive law for the CFpm model.",
		  ((bool,preventGranularRatcheting,false,"If true rotations are computed such as granular ratcheting is prevented. See article [Alonso2004]_, pg. 3-10 -- and a lot more papers from the same authors)."))
	);
	DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM);
