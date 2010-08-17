/**

=== HIGH LEVEL OVERVIEW OF RockPM ===

Rock Particle Model (RockPM) is a set of classes for modelling
mechanical behavior of mining rocks.
*/

#pragma once

#include<yade/pkg-common/Dispatching.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/pkg-common/ElastMat.hpp>


class RpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmState,State,"State information about Rpm body.",
		((int,specimenNumber,0,"The variable is used for particle size distribution analyze. Indicates, to which part of specimen belongs para of particles."))
		((Real,specimenMass,0,"Indicates the mass of the whole stone, which owns the particle."))
		((Real,maxDiametrParticle,0,"Indicates the maximal diametr of the specimen.")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(RpmState,State);
};
REGISTER_SERIALIZABLE(RpmState);

class Law2_Dem3DofGeom_RockPMPhys_Rpm: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I);
		FUNCTOR2D(Dem3DofGeom,RpmPhys);
		
	YADE_CLASS_BASE_DOC(Law2_Dem3DofGeom_RockPMPhys_Rpm,LawFunctor,"Constitutive law for the Rpm model");
	DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_RockPMPhys_Rpm);

/** This class holds information associated with each body */
class RpmMat: public FrictMat {
		public:
			virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new RpmState); }
			virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<RpmState*>(s); }
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmMat,FrictMat,"Rock material, for use with other Rpm classes.",
			((int,exampleNumber,0,"Number of the specimen. This value is equal for all particles of one specimen. [-]"))
			((bool,initCohesive,false,"The flag shows, whether particles of this material can be cohesive. [-]"))
			((Real,stressCompressMax,0,"Maximal strength for compression. The main destruction parameter. [Pa] //(Needs to be reworked)"))
			((Real,Brittleness,0,"One of destruction parameters. [-] //(Needs to be reworked)"))
			((Real,G_over_E,1,"Ratio of normal/shear stiffness at interaction level. [-]")),
			createIndex();
			);
		
		REGISTER_CLASS_INDEX(RpmMat,FrictMat);
};
REGISTER_SERIALIZABLE(RpmMat);


class Ip2_RpmMat_RpmMat_RpmPhys: public InteractionPhysicsFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		FUNCTOR2D(RpmMat,RpmMat);
		DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_RpmMat_RpmMat_RpmPhys,InteractionPhysicsFunctor,"Convert 2 RpmMat instances to RpmPhys with corresponding parameters.",
		((Real,initDistance,0,"Initial distance between spheres at the first step."))
	);
};
REGISTER_SERIALIZABLE(Ip2_RpmMat_RpmMat_RpmPhys);


class RpmPhys: public NormShearPhys {
	private:
	public:
		Real omega, Fn, sigmaN, epsN;
		Vector3r epsT, sigmaT, Fs;
		 
		virtual ~RpmPhys();

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmPhys,NormShearPhys,"Representation of a single interaction of the Cpm type: storage for relevant parameters.\n\n Evolution of the contact is governed by Law2_Dem3DofGeom_CpmPhys_Cpm, that includes damage effects and chages of parameters inside CpmPhys",
			((Real,E,NaN,"normal modulus (stiffness / crossSection) [Pa]"))
			((Real,crossSection,0,"equivalent cross-section associated with this contact [m²]"))
			((Real,G,NaN,"shear modulus [Pa]"))
			((Real,tanFrictionAngle,NaN,"tangens of internal friction angle [-]"))
			((bool,isCohesive,false,"if not cohesive, interaction is deleted when distance is greater than lengthMaxTension or less than lengthMaxCompression."))
			((Real,lengthMaxCompression,0,"Maximal penetration of particles during compression. If it is more, the interaction is deleted [m]"))
			((Real,lengthMaxTension,0,"Maximal distance between particles during tension. If it is more, the interaction is deleted [m]"))
			,
			/*ctor*/createIndex(); epsT=Vector3r::Zero(); omega=0; Fn=0; Fs=Vector3r::Zero();
		);
	REGISTER_CLASS_INDEX(RpmPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(RpmPhys);
