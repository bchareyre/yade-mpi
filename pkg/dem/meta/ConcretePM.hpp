// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 

/*

=== HIGH LEVEL OVERVIEW OF CPM ===

Concrete Particle Model (ConcretePM, Cpm) is a set of classes for modelling
mechanical behavior of concrete. Several classes are needed for Cpm.

1. CpmMat (Cpm material) deriving from BodyMacroParameters, which additionally has
   some information about damage on the body, cummulative plastic strain etc.

2.	Ip2_CpmMat_CpmMat_CpmPhys is 2-ary functor for creating CpmPhys from CpmMat's of
	2 bodies that collide. Some parameters of the CpmPhys created are computed from
	CpmMat's, others are passed as parameters of the functor.

3. CpmPhys (Cpm (interaction)Physics) holds various parameters as well as internal
   variables of the contact that can change as result of plasticity, damage, viscosity.

4. Law2_Dem3Dof_CpmPhys_Cpm is constitutive law that takes geometry of the interaction
	(Dem3Dof, which can be either Dem3Dof_SphereSphere or Dem3Dof_FacetSphere) and
	CpmPhys, computing forces on both bodies and updating contact variables.

	The model itself is defined in the macro CPM_MATERIAL_MODEL, but due to 
	commercial reasons, those about 30 lines of code cannot be disclosed now and the macro
	is defined in an external file. The model will be, however, described in enough detail
	in my thesis (once it is written), along
	with calibration procedures; it features damage, plasticity and viscosity
	and is quite tunable (rigidity, poisson's	ratio, compressive/tensile strength
	ratio, fracture energy, behavior under confinement, rate-dependence).

There are other classes, which are not strictly necessary:

 * CpmGlobalCharacteristics computes a few information about individual bodies based on
   interactions they are involved in. It is probably quite useless now since volumetricStrain
	is not used in the constitutive law anymore.

 * GLDrawCpmPhys draws interaction physics (color for damage and a few other); rarely used, though.

 * CpmStateUpdater changes bodies' colors depending on average damage of their interactions
   and number of interactions that were already fully broken and have disappeared. This engine
	contains its own loop (2 loops, more precisely) over all bodies and is run periodically
	to update colors.

*/

#pragma once

#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-common/ConstitutiveLaw.hpp>

/* This class holds information associated with each body */
class CpmMat: public BodyMacroParameters {
	public:
		//! volumetric strain around this body
		Real epsVolumetric;
		//! number of (cohesive) contacts that damaged completely
		int numBrokenCohesive;
		//! number of contacts with this body
		int numContacts;
		//! average damage including already deleted contacts (it is really not damage, but 1-relResidualStrength now)
		Real normDmg;
		//! plastic strain on contacts already deleted
		Real epsPlBroken;
		//! sum of plastic strains normalized by number of contacts
		Real normEpsPl;
		//! stresses on the particle
		Vector3r sigma,tau;
		CpmMat(): epsVolumetric(0.), numBrokenCohesive(0), numContacts(0), normDmg(0.), epsPlBroken(0.), normEpsPl(0.), sigma(Vector3r::ZERO), tau(Vector3r::ZERO) {createIndex();};
		REGISTER_ATTRIBUTES(BodyMacroParameters, (epsVolumetric) (numBrokenCohesive) (numContacts) (normDmg) (epsPlBroken) (normEpsPl) (sigma) (tau));
		REGISTER_CLASS_AND_BASE(CpmMat,BodyMacroParameters);
		REGISTER_CLASS_INDEX(CpmMat,BodyMacroParameters);
};
REGISTER_SERIALIZABLE(CpmMat);


/*! @brief representation of a single interaction of the CPM type: storage for relevant parameters.
 *
 * Evolution of the contact is governed by Law2_Dem3DofGeom_CpmPhys_Cpm:
 * that includes damage effects and chages of parameters inside CpmPhys.
 *
 */
class CpmPhys: public NormalShearInteraction {
	private:
	public:
		/*! Fundamental parameters (constants) */
		Real
			//! normal modulus (stiffness / crossSection)
			E,
			//! shear modulus
			G,
			//! tangens of internal friction angle
			tanFrictionAngle, 
			//! virgin material cohesion
			undamagedCohesion,
			//! equivalent cross-section associated with this contact
			crossSection,
			//! strain at which the material starts to behave non-linearly
			epsCrackOnset,
			//! strain where the damage-evolution law tangent from the top (epsCrackOnset) touches the axis;
			/// since the softening law is exponential, this doesn't mean that the contact is fully damaged at this point,
			/// that happens only asymptotically 
			epsFracture,
			//! characteristic time for damage (if non-positive, the law without rate-dependence is used)
			dmgTau,
			//! exponent in the rate-dependent damage evolution
			dmgRateExp,
			//! damage strain (at previous or current step)
			dmgStrain,
			//! damage viscous overstress (at previous step or at current step)
			dmgOverstress,
			//! characteristic time for viscoplasticity (if non-positive, no rate-dependence for shear)
			plTau,
			//! exponent in the rate-dependent viscoplasticity
			plRateExp,
			//! "prestress" of this link (used to simulate isotropic stress)
			isoPrestress;
		/*! Up to now maximum normal strain (semi-norm), non-decreasing in time. */
		Real kappaD;
		//! normal plastic strain (initially zero)
		Real epsNPl;
		/*! Transversal strain (perpendicular to the contact axis) */
		Real epsTrans;
		/*! if not cohesive, interaction is deleted when distance is greater than zero. */
		bool isCohesive;
		/*! the damage evlution function will always return virgin state */
		bool neverDamage;
		/*! cummulative shear plastic strain measure (scalar) on this contact */
		Real epsPlSum;
		//! debugging, to see convergence rate
		static long cummBetaIter, cummBetaCount;

		/*! auxiliary variable for visualization, recalculated in Law2_Dem3DofGeom_CpmPhys_Cpm at every iteration */
		// Fn and Fs are also stored as Vector3r normalForce, shearForce in NormalShearInteraction 
		Real omega, Fn, sigmaN, epsN, relResidualStrength; Vector3r epsT, sigmaT, Fs;


		static Real solveBeta(const Real c, const Real N);
		Real computeDmgOverstress(Real dt);
		Real computeViscoplScalingFactor(Real sigmaTNorm, Real sigmaTYield,Real dt);



		CpmPhys(): NormalShearInteraction(),E(0), G(0), tanFrictionAngle(0), undamagedCohesion(0), crossSection(0), dmgTau(-1), dmgRateExp(0), dmgStrain(0), plTau(-1), plRateExp(0), isoPrestress(0.), kappaD(0.), epsNPl(0.), epsTrans(0.), epsPlSum(0.) { createIndex(); epsT=Vector3r::ZERO; isCohesive=false; neverDamage=false; omega=0; Fn=0; Fs=Vector3r::ZERO; epsPlSum=0; dmgOverstress=0; }
		virtual ~CpmPhys();

		REGISTER_ATTRIBUTES(NormalShearInteraction,
			(E)
			(G)
			(tanFrictionAngle)
			(undamagedCohesion)
			(crossSection)
			(epsCrackOnset)
			(epsFracture)
			(dmgTau)
			(dmgRateExp)
			(dmgStrain)
			(dmgOverstress)
			(plTau)
			(plRateExp)
			(isoPrestress)

			(cummBetaIter)
			(cummBetaCount)

			(kappaD)
			(epsNPl)
			(neverDamage)
			(epsT)
			(epsTrans)
			(epsPlSum)

			(isCohesive)

			// auxiliary params to make them accessible from python
			(omega)
			(Fn)
			(Fs)
			(epsN)
			(sigmaN)
			(sigmaT)
			(relResidualStrength)
		);
	REGISTER_CLASS_AND_BASE(CpmPhys,NormalShearInteraction);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(CpmPhys,NormalShearInteraction);
};
REGISTER_SERIALIZABLE(CpmPhys);


/*! @brief Convert macroscopic properties to CpmPhys with corresponding parameters.
 *
 * */
class Ip2_CpmMat_CpmMat_CpmPhys: public InteractionPhysicsEngineUnit{
	private:
	public:
		/* nonelastic material parameters */
		/* alternatively (and more cleanly), we would have subclass of ElasticBodyParameters,
		 * which would define just those in addition to the elastic ones.
		 * This might be done later, for now hardcode that here. */
		/* uniaxial tension resistance, bending parameter of the damage evolution law, whear weighting constant for epsT in the strain seminorm (kappa) calculation. Default to NaN so that user gets loudly notified it was not set.
		
		*/
		Real sigmaT, epsCrackOnset, relDuctility, G_over_E, tau, expDmgRate, dmgTau, dmgRateExp, plTau, plRateExp, isoPrestress;
		//! Should new contacts be cohesive? They will before this iter#, they will not be afterwards. If 0, they will never be. If negative, they will always be created as cohesive.
		long cohesiveThresholdIter;
		//! Create contacts that don't receive any damage (CpmPhys::neverDamage=true); defaults to false
		bool neverDamage;

		Ip2_CpmMat_CpmMat_CpmPhys(){
			// init to signaling_NaN to force crash if not initialized (better than unknowingly using garbage values)
			sigmaT=epsCrackOnset=relDuctility=G_over_E=std::numeric_limits<Real>::signaling_NaN();
			neverDamage=false;
			cohesiveThresholdIter=10;
			dmgTau=-1; dmgRateExp=0; plTau=-1; plRateExp=-1;
			isoPrestress=0;
		}

		virtual void go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction);
		REGISTER_ATTRIBUTES(InteractionPhysicsEngineUnit,
			(cohesiveThresholdIter)
			(G_over_E)
			(sigmaT)
			(neverDamage)
			(epsCrackOnset)
			(relDuctility)
			(dmgTau)
			(dmgRateExp)
			(plTau)
			(plRateExp)
			(isoPrestress)
		);

		FUNCTOR2D(CpmMat,CpmMat);
		REGISTER_CLASS_AND_BASE(Ip2_CpmMat_CpmMat_CpmPhys,InteractionPhysicsEngineUnit);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_CpmMat_CpmMat_CpmPhys);



class Law2_Dem3DofGeom_CpmPhys_Cpm: public ConstitutiveLaw{
	public:
	/*! Damage evolution law */
	static Real funcG(const Real& kappaD, const Real& epsCrackOnset, const Real& epsFracture, const bool& neverDamage) {
		if(kappaD<epsCrackOnset || neverDamage) return 0;
		return 1.-(epsCrackOnset/kappaD)*exp(-(kappaD-epsCrackOnset)/epsFracture);
	}
		//! yield function: 0: mohr-coulomb (original); 1: parabolic; 2: logarithmic, 3: log+lin_tension, 4: elliptic, 5: elliptic+log
		static int yieldSurfType;
		//! scaling in the logarithmic yield surface (should be <1 for realistic results; >=0 for meaningful results)
		static Real yieldLogSpeed;
		//! horizontal scaling of the ellipse (shifts on the +x axis as interactions with +y are given)
		static Real yieldEllipseShift;
		//! damage after which the contact disappears (<1), since omega reaches 1 only for strain →+∞
		static Real omegaThreshold;
		//! Strain at which softening in compression starts (set to 0. (default) or positive value to deactivate)
		static Real epsSoft;
		//! Relative rigidity of the softening branch in compression (0=perfect elastic-plastic, 1=no softening, >1=hardening)
		static Real relKnSoft;
		Law2_Dem3DofGeom_CpmPhys_Cpm() { }
		void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody);
		// utility functions
		//! Update avgStress on all bodies (called from VTKRecorder and yade.eudoxos.particleConfinement)
		//! Might be anywhere else as well (static method)
		static void updateBodiesState(MetaBody*);
	FUNCTOR2D(Dem3DofGeom,CpmPhys);
	REGISTER_CLASS_AND_BASE(Law2_Dem3DofGeom_CpmPhys_Cpm,ConstitutiveLaw);
	REGISTER_ATTRIBUTES(ConstitutiveLaw,(yieldSurfType)(yieldLogSpeed)(yieldEllipseShift)(omegaThreshold)(epsSoft)(relKnSoft));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_CpmPhys_Cpm);

//deprecated code
#if 0
/* Engine encompassing several computations looping over all bodies/interactions
 *
 * * Compute and store unbalanced force over the whole simulation.
 * * Compute and store volumetric strain for every body.
 *
 * May be extended in the future to compute global stiffness etc as well.
 */
class CpmGlobalCharacteristics: public PeriodicEngine{
	public:
		bool useMaxForce; // use maximum unbalanced force instead of mean unbalanced force
		Real unbalancedForce;
		void compute(MetaBody* rb, bool useMax=false);
		virtual void action(MetaBody* rb){compute(rb,useMaxForce);}
		CpmGlobalCharacteristics(){};
	REGISTER_ATTRIBUTES(PeriodicEngine,
		(unbalancedForce)
		(useMaxForce)
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(CpmGlobalCharacteristics,PeriodicEngine);
};
REGISTER_SERIALIZABLE(CpmGlobalCharacteristics);
#endif

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	class GLDrawCpmPhys: public GLDrawInteractionPhysicsFunctor {
		public: virtual void go(const shared_ptr<InteractionPhysics>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
		virtual ~GLDrawCpmPhys() {};
		REGISTER_ATTRIBUTES(Serializable,(contactLine)(dmgLabel)(dmgPlane)(epsT)(epsTAxes)(normal)(colorStrain)(epsNLabel));
		RENDERS(CpmPhys);
		REGISTER_CLASS_AND_BASE(GLDrawCpmPhys,GLDrawInteractionPhysicsFunctor);
		DECLARE_LOGGER;
		static bool contactLine,dmgLabel,dmgPlane,epsT,epsTAxes,normal,colorStrain,epsNLabel;
	};
	REGISTER_SERIALIZABLE(GLDrawCpmPhys);
#endif

class CpmStateUpdater: public PeriodicEngine {
	struct BodyStats{ int nCohLinks; int nLinks; Real dmgSum, epsPlSum; Vector3r sigma, tau; BodyStats(): nCohLinks(0), nLinks(0), dmgSum(0.), epsPlSum(0.), sigma(Vector3r::ZERO), tau(Vector3r::ZERO) {} };
	public:
		//! maximum damage over all contacts
		static Real maxOmega;
		CpmStateUpdater(){maxOmega=0; /* run at the very beginning */ initRun=true;}
		virtual void action(MetaBody* rb){ update(rb); }
		static void update(MetaBody* rb=NULL);
	DECLARE_LOGGER;
	REGISTER_ATTRIBUTES(PeriodicEngine,(maxOmega));
	REGISTER_CLASS_AND_BASE(CpmStateUpdater,PeriodicEngine);
};
REGISTER_SERIALIZABLE(CpmStateUpdater);

