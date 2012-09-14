/**

=== HIGH LEVEL OVERVIEW OF RockPM ===

Rock Particle Model (RockPM) is a set of classes for modelling
mechanical behavior of mining rocks.
*/

#pragma once

#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/common/PeriodicEngines.hpp>
#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/common/ElastMat.hpp>

class RpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmState,State,"State information about Rpm body.",
		((int,specimenNumber,0,,"The variable is used for particle size distribution analyze. Indicates, to which part of specimen belongs para of particles."))
		((Real,specimenMass,0,,"Indicates the mass of the whole stone, which owns the particle."))
		((Real,specimenVol,0,,"Indicates the mass of the whole stone, which owns the particle."))
		((Real,specimenMaxDiam,0,,"Indicates the maximal diametr of the specimen.")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(RpmState,State);
};
REGISTER_SERIALIZABLE(RpmState);

/** This class holds information associated with each body */
class RpmMat: public FrictMat {
		public:
			virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new RpmState); }
			virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<RpmState*>(s); }
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmMat,FrictMat,"Rock material, for use with other Rpm classes.",
			((int,exampleNumber,0,,"Number of the specimen. This value is equal for all particles of one specimen. [-]"))
			((bool,initCohesive,false,,"The flag shows, whether particles of this material can be cohesive. [-]"))
			((Real,stressCompressMax,0,,"Maximal strength for compression. The main destruction parameter. [Pa] //(Needs to be reworked)"))
			((Real,stressStretchingMax,0,,"Maximal strength for stretching. [Pa]"))
			((Real,stressShearMax,0,,"Maximal strength for shearing. [Pa]"))
			((Real,G_over_E,1,,"Ratio of normal/shear stiffness at interaction level. [-]"))
			((Real,Zeta,0,,"Damping Ratio, http://en.wikipedia.org/wiki/Damping_ratio [-]")),
			createIndex();
			);
		
		REGISTER_CLASS_INDEX(RpmMat,FrictMat);
};
REGISTER_SERIALIZABLE(RpmMat);


class Ip2_RpmMat_RpmMat_RpmPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		FUNCTOR2D(RpmMat,RpmMat);
		DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_RpmMat_RpmMat_RpmPhys,IPhysFunctor,"Convert 2 RpmMat instances to RpmPhys with corresponding parameters.",);
};
REGISTER_SERIALIZABLE(Ip2_RpmMat_RpmMat_RpmPhys);


class RpmPhys: public NormShearPhys {
	private:
	public:
		Real Fn,  epsN, sigmaN, cn, cs;
		Vector3r epsT,  Fs, sigmaT;
		bool updatedCnFlag;
		 
		virtual ~RpmPhys();

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(RpmPhys,NormShearPhys,"Representation of a single interaction of the Rpm type: storage for relevant parameters.\n\n Evolution of the contact is governed by Law2_Dem3DofGeom_RpmPhys_Rpm, that includes damage effects and chages of parameters inside RpmPhys",
			((Real,E,NaN,,"normal modulus (stiffness / crossSection) [Pa]"))
			((Real,crossSection,0,,"equivalent cross-section associated with this contact [m²]"))
			((Real,G,NaN,,"shear modulus [Pa]"))
			((Real,tanFrictionAngle,NaN,,"tangens of internal friction angle [-]"))
			((bool,isCohesive,false,,"if not cohesive, interaction is deleted when distance is greater than lengthMaxTension or less than lengthMaxCompression."))
			((Real,epsMaxCompression,0,,"Maximal relative penetration of particles during compression. If it is more, the interaction is deleted [m]"))
			((Real,epsMaxTension,0,,"Maximal relative distance between particles during tension. If it is more, the interaction is deleted [m]"))
			((Real,epsMaxShear,0,,"Maximal relative distance between particles in tangential direction. If it is more, the interaction is deleted [m]"))
			((Real,Kn,0,,"Stiffness in normal direction [N/m]"))
			((Real,Ks,0,,"Stiffness in shear direction [N/m]")),
			/*ctor*/createIndex(); Fn=0; epsN=0; sigmaN=0; sigmaT = Vector3r::Zero(); epsT=Vector3r::Zero(); Fs=Vector3r::Zero(); cn=0; cs=0;
		);
	REGISTER_CLASS_INDEX(RpmPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(RpmPhys);

class Law2_Dem3DofGeom_RockPMPhys_Rpm: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(Dem3DofGeom,RpmPhys);
		
	YADE_CLASS_BASE_DOC(Law2_Dem3DofGeom_RockPMPhys_Rpm,LawFunctor,"Constitutive law for the Rpm model");
	DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_RockPMPhys_Rpm);


