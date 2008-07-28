// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/extra/Shop.hpp>

#include<yade/core/InteractionSolver.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/GLDrawFunctors.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-dem/GlobalStiffness.hpp>

// #define BREFCOM_REC

/*! @brief representation of a single interaction of the brefcom type: storage for relevant parameters.
 *
 * Evolution of the contact is governed by BrefcomLaw:
 * that includes damage effects and chages of parameters inside BrefcomContact.
 *
 */
class BrefcomContact: public InteractionPhysics {
	private:
	public:
		/*! Fundamental parameters (constants) */
		Real E, G, tanFrictionAngle, undamagedCohesion, equilibriumDist, crossSection, epsCrackOnset, epsFracture, expBending, xiShear, tau, expDmgRate;
		/*! Up to now maximum normal strain */
		Real kappaD;
		/*! prevNormal is oriented A→B (as in SpheresContactGeometry); */
		Vector3r prevNormal;
		/*! previous tangential (shear) strain */
		Vector3r epsT;
		/*! if not cohesive, interaction is deleted when distance is greater than zero. */
		bool isCohesive;
		/*! the damage evlution function will always return virgin state */
		bool neverDamage;

		/*! auxiliary variable for visualization and BrefcomStiffnessCounter, recalculated by BrefcomLaw at every iteration */
		Real omega, Fn, sigmaN, epsN; Vector3r sigmaT, Fs;

		BrefcomContact(): InteractionPhysics(),E(0), G(0), tanFrictionAngle(0), undamagedCohesion(0), equilibriumDist(0), crossSection(0), expBending(0), xiShear(0), tau(0), expDmgRate(0) { createIndex(); epsT=Vector3r::ZERO; kappaD=0; isCohesive=false; }
		BrefcomContact(Real _E, Real _G, Real _tanFrictionAngle, Real _undamagedCohesion, Real _equilibriumDist, Real _crossSection, Real _epsCrackOnset, Real _epsFracture, Real _expBending, Real _xiShear, Real _tau=0, Real _expDmgRate=1): InteractionPhysics(), E(_E), G(_G), tanFrictionAngle(_tanFrictionAngle), undamagedCohesion(_undamagedCohesion), equilibriumDist(_equilibriumDist), crossSection(_crossSection), epsCrackOnset(_epsCrackOnset), epsFracture(_epsFracture), expBending(_expBending), xiShear(_xiShear), tau(_tau), expDmgRate(_expDmgRate) { epsT=Vector3r::ZERO; kappaD=0; isCohesive=false; neverDamage=false; omega=0; Fn=0; Fs=Vector3r::ZERO; /*TRVAR5(epsCrackOnset,epsFracture,Kn,crossSection,equilibriumDist); */ }


		void registerAttributes(){
			InteractionPhysics::registerAttributes();
			REGISTER_ATTRIBUTE(E);
			REGISTER_ATTRIBUTE(G);
			REGISTER_ATTRIBUTE(tanFrictionAngle);
			REGISTER_ATTRIBUTE(undamagedCohesion);
			REGISTER_ATTRIBUTE(equilibriumDist);
			REGISTER_ATTRIBUTE(crossSection);
			REGISTER_ATTRIBUTE(epsCrackOnset);
			REGISTER_ATTRIBUTE(epsFracture);
			REGISTER_ATTRIBUTE(expBending);
			REGISTER_ATTRIBUTE(xiShear);
			REGISTER_ATTRIBUTE(tau);
			REGISTER_ATTRIBUTE(expDmgRate);

			REGISTER_ATTRIBUTE(kappaD);
			REGISTER_ATTRIBUTE(neverDamage);
			REGISTER_ATTRIBUTE(prevNormal);
			REGISTER_ATTRIBUTE(epsT);

			REGISTER_ATTRIBUTE(isCohesive);

			// auxiliary params, to make them accessible from python
			REGISTER_ATTRIBUTE(omega);
			REGISTER_ATTRIBUTE(Fn);
			REGISTER_ATTRIBUTE(Fs);
			REGISTER_ATTRIBUTE(epsN);
			REGISTER_ATTRIBUTE(sigmaN);
			REGISTER_ATTRIBUTE(sigmaT);
		};

	REGISTER_CLASS_NAME(BrefcomContact);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(BrefcomContact,false);

#define BREFREC(a) {recValues.push_back(a); recLabels.push_back(#a);}
#define BREFREC2(a,b) {recValues.push_back(a); recLabels.push_back(b);}
class BrefcomLaw: public InteractionSolver{
	private:
		// cache class indices to avoid lookup at every iteration
		int MomentumClassIndex, ForceClassIndex; // if possible make it "const int" and move init to initializers
		//! aplly calculated force on both particles (applied in the inverse sense on B)
		void applyForces();
		/* storage variables, to avoid passing them around on the stack.
		 *
		 * The orientation is conventionally as forces should be applied on the FIRST element in the interaction.
		 */
		//Vector3r Fs; Real Fn;
		/* temporaries initialized in the loop over interactions; above methods use these. */
		body_id_t id1, id2;
		shared_ptr<Body> body1, body2;
		shared_ptr<BrefcomContact> BC;
		shared_ptr<SpheresContactGeometry> contGeom;
		shared_ptr<RigidBodyParameters> rbp1, rbp2;
		MetaBody* rootBody;
		// recording  values
		#ifdef BREFCOM_REC
			ofstream recStream;
			vector<Real> recValues;
			vector<string> recLabels;
		#endif
		void applyForce(const Vector3r);
		/*! Cohesion evolution law (it is 1-funcH here) */
		inline Real funcH(Real kappaD){ return 1-funcG(kappaD); }
		/*! Damage evolution law */
		inline Real funcG(Real kappaD){
			//const Real& expBending=BC->expBending, 
			const Real& epsCrackOnset=BC->epsCrackOnset, epsFracture=BC->epsFracture; const bool& neverDamage=BC->neverDamage; // shorthands
			if(kappaD<epsCrackOnset || neverDamage) return 0;
			return 1.-(epsCrackOnset/kappaD)*exp(-(kappaD-epsCrackOnset)/epsFracture);
			//if(kappaD>epsFracture) return 1;
			//return (1/(1-exp(-expBending)))*(1-exp(-expBending*(kappaD-epsCrackOnset)/(epsFracture-epsCrackOnset)));
		}
		
	public:
		#if 0
			// static versions: for calibration of parameters without any existing instance
			static Real damageEvolutionLaw_static(Real kappaD, Real expBending, Real epsCrackOnset, Real epsFracture);
			static Real unitFractureEnergy(double expBending, double epsCrackOnset, double epsFracture, int steps=50);
			static Real calibrateEpsFracture(double Gf, double E, double expBending, double epsCrackOnset, double epsFractureInit=1e-3, double relEpsilon=1e-3, int maxIter=1000);
		#endif
		BrefcomLaw() {
			/* cache indices for Force and Momentum */
			//ForceClassIndex=shared_ptr<PhysicalAction>(new Force())->getClassIndex();
			//MomentumClassIndex=shared_ptr<PhysicalAction>(new Momentum())->getClassIndex();
			Shop::Bex::initCache();
		};
		void action(MetaBody*);
	protected: 
		void registerAttributes(){InteractionSolver::registerAttributes();};
		void postProcessAttributes(bool deserializing){ if(deserializing)
		#ifdef BREFCOM_REC
			recStream.open("/tmp/breflaw.data")
		#endif
		; }
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(BrefcomLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(BrefcomLaw,false);

/*! @brief Convert macroscopic properties to BrefcomContact with corresponding parameters.
 *
 * Most of the model is taken from MacroMicroElasticRelationships.
 *
 * @todo alpha, beta, gamma constants should be set to better values that Laurent presented in Nantes.
 *
 * */
class BrefcomMakeContact: public InteractionPhysicsEngineUnit{
	private:
	public:
		/* "constants" for macro-micro: for the algorithm, see comments in code of BrefcomMakeContact::go; values of these constants are based on CFRAC 2007 proceedings. */
		Real alpha, beta, gamma;
		/* nonelastic material parameters */
		/* alternatively (and more cleanly), we would have subclass of ElasticBodyParameters,
		 * which would define just those in addition to the elastic ones.
		 * This might be done later, for now hardcode that here. */
		/* uniaxial tension resistance, bending parameter of the damage evolution law, whear weighting constant for epsT in the strain seminorm (kappa) calculation. Default to NaN so that user gets loudly notified it was not set.
		
		expBending is positive if the damage evolution function is concave after fracture onset;
		reasonable value seems like 4.
		*/
		Real sigmaT, expBending, xiShear, epsCrackOnset, relDuctility, G_over_E, tau, expDmgRate;
		//! Should new contacts be cohesive? They will before this iter#, they will not be afterwards. If 0, they will never be. If negative, they will always be created as cohesive.
		long cohesiveThresholdIter;
		//! Create contacts that don't receive any damage (BrefcomContact::neverDamage=true); defaults to false
		bool neverDamage;

		BrefcomMakeContact(){
			// init to signaling_NaN to force crash if not initialized (better than unknowingly using garbage values)
			sigmaT=expBending=xiShear=epsCrackOnset=relDuctility=G_over_E=std::numeric_limits<Real>::signaling_NaN();
			neverDamage=false;
			alpha=3.7; beta=2.198; gamma=3.79; // Laurent's defaults
			cohesiveThresholdIter=-1;
			tau=-1; expDmgRate=0;
		}

		virtual void go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction);
		virtual void registerAttributes(){
			InteractionPhysicsEngineUnit::registerAttributes();
			REGISTER_ATTRIBUTE(cohesiveThresholdIter);
			REGISTER_ATTRIBUTE(alpha);
			REGISTER_ATTRIBUTE(beta);
			REGISTER_ATTRIBUTE(gamma);

			REGISTER_ATTRIBUTE(G_over_E);
			REGISTER_ATTRIBUTE(expBending);
			REGISTER_ATTRIBUTE(xiShear);
			REGISTER_ATTRIBUTE(sigmaT);
			REGISTER_ATTRIBUTE(neverDamage);
			REGISTER_ATTRIBUTE(epsCrackOnset);
			REGISTER_ATTRIBUTE(relDuctility);
			REGISTER_ATTRIBUTE(tau);
			REGISTER_ATTRIBUTE(expDmgRate);
			//REGISTER_ATTRIBUTE(calibratedEpsFracture);
			/* REGISTER_ATTRIBUTE(Gf); */
		}

		FUNCTOR2D(BodyMacroParameters,BodyMacroParameters);
		REGISTER_CLASS_NAME(BrefcomMakeContact);
		REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(BrefcomMakeContact,false);

class GLDrawBrefcomContact: public GLDrawInteractionPhysicsFunctor {
	public: virtual void go(const shared_ptr<InteractionPhysics>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
	virtual ~GLDrawBrefcomContact() {};
	virtual void registerAttributes(){ REGISTER_ATTRIBUTE(contactLine); REGISTER_ATTRIBUTE(dmgLabel); REGISTER_ATTRIBUTE(epsT); REGISTER_ATTRIBUTE(epsTAxes); REGISTER_ATTRIBUTE(normal); }
	RENDERS(BrefcomContact);
	REGISTER_CLASS_NAME(GLDrawBrefcomContact);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
	DECLARE_LOGGER;
	static bool contactLine,dmgLabel,epsT,epsTAxes,normal;
};
REGISTER_SERIALIZABLE(GLDrawBrefcomContact,false);

class BrefcomDamageColorizer : public PeriodicEngine {
	public :
		BrefcomDamageColorizer(){}
		virtual void action(MetaBody*);
	virtual void registerAtributes(){PeriodicEngine::registerAttributes();}
	REGISTER_CLASS_NAME(BrefcomDamageColorizer);
	REGISTER_BASE_CLASS_NAME(PeriodicEngine);
};
REGISTER_SERIALIZABLE(BrefcomDamageColorizer,false);


/*** !!! BADLY BROKEN !!! Do not use !!! ***/
#if 0
class BrefcomStiffnessCounter : public InteractionSolver{
	private :
		int actionStiffnessIndex;
	public:
		unsigned int interval;
		BrefcomStiffnessCounter() {interval=500; actionStiffnessIndex=shared_ptr<PhysicalAction>(new GlobalStiffness)->getClassIndex();}
		void action(MetaBody*);
		virtual bool isActivated(){return (Omega::instance().getCurrentIteration()%interval)==0;}
	protected :
		void registerAttributes(){REGISTER_ATTRIBUTE(interval);}
	NEEDS_BEX("Force","Momentum","GlobalStiffness");
	REGISTER_CLASS_NAME(BrefcomStiffnessCounter);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(BrefcomStiffnessCounter,false);
#endif
 


