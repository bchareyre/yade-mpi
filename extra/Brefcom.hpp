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
#include<yade/pkg-common/GLDrawInteractionPhysicsFunctor.hpp>
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
		Real E, G, tanFrictionAngle, undamagedCohesion, equilibriumDist, crossSection, epsCracking, epsFracture, expBending, xiShear;
		/*! Up to now maximum normal strain */
		Real kappaD;
		/*! prevNormal is oriented A→B (as in SpheresContactGeometry); */
		Vector3r prevNormal;
		/*! previous tangential (shear) strain */
		Vector3r epsT;
		/*! if not cohesive, interaction is deleted when distance is greater than zero. */
		bool isCohesive;

		/*! auxiliary variable for visualization and BrefcomStiffnessCounter, recalculated by BrefcomLaw at every iteration */
		Real omega, Fn, sigmaN, epsN;

		BrefcomContact(): InteractionPhysics(),E(0), G(0), tanFrictionAngle(0), undamagedCohesion(0), equilibriumDist(0), crossSection(0), expBending(0), xiShear(0) { createIndex(); epsT=Vector3r::ZERO; kappaD=0; isCohesive=false; }
		BrefcomContact(Real _E, Real _G, Real _tanFrictionAngle, Real _undamagedCohesion, Real _equilibriumDist, Real _crossSection, Real _epsCracking, Real _epsFracture, Real _expBending, Real _xiShear=.3): InteractionPhysics(), E(_E), G(_G), tanFrictionAngle(_tanFrictionAngle), undamagedCohesion(_undamagedCohesion), equilibriumDist(_equilibriumDist), crossSection(_crossSection), epsCracking(_epsCracking), epsFracture(_epsFracture), expBending(_expBending), xiShear(_xiShear) { epsT=Vector3r::ZERO; kappaD=0; isCohesive=false; omega=0; Fn=0; /*TRVAR5(epsCracking,epsFracture,Kn,crossSection,equilibriumDist); */ }


		void registerAttributes(){
			InteractionPhysics::registerAttributes();
			REGISTER_ATTRIBUTE(E);
			REGISTER_ATTRIBUTE(G);
			REGISTER_ATTRIBUTE(tanFrictionAngle);
			REGISTER_ATTRIBUTE(undamagedCohesion);
			REGISTER_ATTRIBUTE(equilibriumDist);
			REGISTER_ATTRIBUTE(crossSection);
			REGISTER_ATTRIBUTE(epsCracking);
			REGISTER_ATTRIBUTE(epsFracture);
			REGISTER_ATTRIBUTE(expBending);
			REGISTER_ATTRIBUTE(xiShear);

			REGISTER_ATTRIBUTE(kappaD);
			REGISTER_ATTRIBUTE(prevNormal);
			REGISTER_ATTRIBUTE(epsT);

			REGISTER_ATTRIBUTE(isCohesive);

			// auxiliary params, to make them accessible from python
			REGISTER_ATTRIBUTE(omega); REGISTER_ATTRIBUTE(Fn);
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
		Vector3r Fs; Real Fn;
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
		Real funcH(Real kappaD){ return 1-funcG(kappaD); }
		/*! Damage evolution law */
		Real funcG(Real kappaD){ return damageEvolutionLaw_static(kappaD, BC->expBending, BC->epsCracking, BC->epsFracture); }

		// static versions: for calibration of parameters without any existing instance
		static Real damageEvolutionLaw_static(Real kappaD, Real expBending, Real epsCrackOnset, Real epsFracture);
		static Real unitFractureEnergy(double expBending, double epsCrackOnset, double epsFracture, int steps=50);
	public:
		static Real calibrateEpsFracture(double Gf, double E, double expBending, double epsCrackOnset, double epsFractureInit=1e-3, double relEpsilon=1e-3, int maxIter=1000);

		BrefcomLaw() {
			/* cache indices for Force and Momentum */
			ForceClassIndex=shared_ptr<PhysicalAction>(new Force())->getClassIndex();
			MomentumClassIndex=shared_ptr<PhysicalAction>(new Momentum())->getClassIndex();
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
		/* uniaxial traction resistance, uniaxial compression resistance, fracture energy, strain at complete fracture (calibrated in the constructor), bending parameter of the damage evolution law */
		Real sigmaT, sigmaC, /* Griffith's fracture energy */ Gf, expBending;
		//! Should new contacts be cohesive? They will before this iter#, they will not be afterwards. If 0, they will never be. If negative, they will always be created as cohesive.
		long cohesiveThresholdIter;

		BrefcomMakeContact(){
			alpha=3.7; beta=2.198; gamma=3.79; sigmaC=30e6; sigmaT=sigmaC/10.;
			expBending=4; /* positive: concave function */
			cohesiveThresholdIter=-1;
			Gf=500; /* J/m^2 */
			#if 0
				/* calibrate epsFracture; WARNING: this is based on the default E value from Shop
				 * and therefore may not match the actual E !! */
				Real E=Shop::getDefault<double>("phys_young");
				Real epsCrackOnset=sigmaT/E;
				try{
					calibratedEpsFracture=BrefcomLaw::calibrateEpsFracture(Gf,E,expBending,epsCrackOnset);
					LOG_DEBUG("calibratedEpsFracture="<<calibratedEpsFracture);
					if(calibratedEpsFracture>epsCrackOnset) LOG_WARN("calibratedEpsFracture="<<calibratedEpsFracture<<" < epsCrackOnset="<<epsCrackOnset<<", Gf="<<Gf<<", E="<<E<<", expBending="<<expBending);
				} catch (std::runtime_error& e){
					LOG_ERROR("Caught exception from calibration: "<<e.what());
					calibratedEpsFracture=3*epsCrackOnset;
					LOG_ERROR("Setting calibratedEpsFracture to "<<calibratedEpsFracture);
				}
			#endif
		}

		virtual void go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction);
		virtual void registerAttributes(){
			InteractionPhysicsEngineUnit::registerAttributes();
			REGISTER_ATTRIBUTE(alpha);
			REGISTER_ATTRIBUTE(beta);
			REGISTER_ATTRIBUTE(gamma);
			REGISTER_ATTRIBUTE(cohesiveThresholdIter);
			//REGISTER_ATTRIBUTE(calibratedEpsFracture);
			REGISTER_ATTRIBUTE(expBending);
			/* REGISTER_ATTRIBUTE(sigmaT);
			REGISTER_ATTRIBUTE(sigmaC);
			REGISTER_ATTRIBUTE(Gf); */
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

class BrefcomDamageColorizer : public StandAloneEngine {
	public :
		int interval;
		BrefcomDamageColorizer(){interval=100;}
		virtual void action(MetaBody*);
		virtual bool isActivated(){return Omega::instance().getCurrentIteration()%interval==0;}
	void registerAttributes(){REGISTER_ATTRIBUTE(interval);}
	REGISTER_CLASS_NAME(BrefcomDamageColorizer);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
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
 


