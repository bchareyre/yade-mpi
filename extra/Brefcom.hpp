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
		/* "constants" for macro-micro */
		Real alpha, beta, gamma;
		/* nonelastic material parameters */
		/* alternatively (and more cleanly), we would have subclass of ElasticBodyParameters,
		 * which would define just those in addition to the elastic ones.
		 * This might be done later, for now hardcode that here. Note that sigmaC and Gf are not used at all now. */
		/* uniaxial traction resistance, uniaxial compression resistance, fracture energy, elastic/softening modulus ratio */
		Real sigmaT, sigmaC, Gf, zeta;

		BrefcomMakeContact(){ alpha=2.5; beta=2.0; gamma=2.65; sigmaT=3e6, sigmaC=30e6; Gf=1e5; zeta=10; }
		virtual void go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction);
		virtual void registerAttributes(){
			InteractionPhysicsEngineUnit::registerAttributes();
			REGISTER_ATTRIBUTE(alpha);
			REGISTER_ATTRIBUTE(beta);
			REGISTER_ATTRIBUTE(gamma);
			REGISTER_ATTRIBUTE(sigmaT);
			REGISTER_ATTRIBUTE(sigmaC);
			REGISTER_ATTRIBUTE(Gf);
			REGISTER_ATTRIBUTE(zeta);
		}
		REGISTER_CLASS_NAME(BrefcomMakeContact);
		REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(BrefcomMakeContact,false);

/*! @brief representation of a single interaction of the brefcom type: storage for relevant parameters.
 *
 * Evolution of the contact is governed by BrefcomLaw:
 * that includes damage effects and chages of parameters inside BrefcomContact.
 *
 * @todo Ks is currently unused, but it is calculated from Kn in BrefcomMakeContact using nu12 and alpha;
 * 	if Kn changes due to damage, how do we adjust Ks? BrefcomMakeContact could do that at every iter? Messy?
 *
 * @todo frictionAngle is currently unused
 */
class BrefcomContact: public InteractionPhysics {
	private:
	public:
		/*! Fundamental parameters (constants) */
		Real d0, Kn, Ks, zeta, frictionAngle, FnMax, cohesion;
		/*! Fundamental state variables */
		Real omegaPl;
		Vector3r prevNormal, Fs; // shear force is cummulative, this must be remembered
		Vector3r Fn; // remembers last normal force, used by stiffness counter
		bool isStructural; // whether this is "neighbour" or just "meeting" contact
		/* calculated by deduceOtherParams (called at every iteration); first two are constants as well, other two depend on damage. */
		Real dPeak, dBreak, d0_curr, dPeak_curr, cohesion_curr, FnMax_curr;
		void deduceOtherParams(void){
			dBreak=d0+(FnMax/Kn)*(1+zeta);
			dPeak=d0+(FnMax/Kn);
			d0_curr=d0+omegaPl*(dBreak-d0);
			dPeak_curr=dPeak+omegaPl*(dBreak-dPeak);
			FnMax_curr=FnMax*(1-omegaPl);
			cohesion_curr=cohesion*(1-omegaPl);
			// if(Omega::instance().getCurrentIteration()%100==0 && omegaPl>=1){ TRVAR4(d0,omegaPl,d0_curr,FnMax_curr); }
		} 

		BrefcomContact(): InteractionPhysics(){ /* just in case someone forgets */ Fs=Vector3r::ZERO; Fn=Vector3r::ZERO; omegaPl=0; isStructural=false; }

		void registerAttributes(){
			InteractionPhysics::registerAttributes();
			REGISTER_ATTRIBUTE(d0);
			REGISTER_ATTRIBUTE(Kn);
			REGISTER_ATTRIBUTE(Ks);
			REGISTER_ATTRIBUTE(zeta);
			REGISTER_ATTRIBUTE(frictionAngle);
			REGISTER_ATTRIBUTE(FnMax);
			REGISTER_ATTRIBUTE(omegaPl);
			REGISTER_ATTRIBUTE(prevNormal);
			REGISTER_ATTRIBUTE(Fs);
			REGISTER_ATTRIBUTE(isStructural);
			/*REGISTER_ATTRIBUTE(dPeak);
			REGISTER_ATTRIBUTE(dBreak);
			REGISTER_ATTRIBUTE(d0_curr);
			REGISTER_ATTRIBUTE(dPeak_curr);*/
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
		//! apply effects of normal displacements
		void calculateNormalForce();
		//! apply effects of shearing
		void calculateShearForce();
		//! apply effects of mutual particle rotation
		void rotationEffects();
		//! adjust forces so that they are not outside resistance envelope and update damage parameter if necessary
		void envelopeAndDamage();
		//! plly calculated force on particles
		void applyForces();
		/* storage variables, to avoid passing them around on the stack */
		Vector3r Fs, Fn;
		/* temporaries initialized in the loop over interactions; above methods use these. */
		body_id_t id1, id2;
		shared_ptr<Body> body1, body2;
		shared_ptr<BrefcomContact> BC;
		shared_ptr<SpheresContactGeometry> contGeom;
		shared_ptr<RigidBodyParameters> rbp1, rbp2;
		MetaBody* rootBody;
		// recording  values
		ofstream recStream;
		vector<Real> recValues;
		vector<string> recLabels;
	public:
		BrefcomLaw() {
			/* cache indices for Force and Momentum */
			ForceClassIndex=shared_ptr<PhysicalAction>(new Force())->getClassIndex();
			MomentumClassIndex=shared_ptr<PhysicalAction>(new Momentum())->getClassIndex();
			TRVAR2(ForceClassIndex,MomentumClassIndex);
		};
		void action(Body* body);
	protected: 
		void registerAttributes(){InteractionSolver::registerAttributes();};
		void postProcessAttributes(bool deserializing){ if(deserializing) recStream.open("/tmp/breflaw.data"); }

	REGISTER_CLASS_NAME(BrefcomLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(BrefcomLaw,false);

 


