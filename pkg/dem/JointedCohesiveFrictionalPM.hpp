/* lucScholtes2010 */

#pragma once

#include<pkg/common/ElastMat.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/NormShearPhys.hpp>
#include<pkg/dem/ScGeom.hpp>
#include <boost/thread/mutex.hpp>
#include <random>

/** This class holds information associated with each body state*/
class JCFpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(JCFpmState,State,"JCFpm state information about each body.",
		((int,nbInitBonds,0,,"Number of initial bonds. [-]"))
		((int,nbBrokenBonds,0,,"Number of broken bonds. [-]"))
		((Real,damageIndex,0,,"Ratio of broken bonds over initial bonds. [-]"))
		((bool,onJoint,false,,"Identifies if the particle is on a joint surface."))
		((int,joint,0,,"Indicates the number of joint surfaces to which the particle belongs (0-> no joint, 1->1 joint, etc..). [-]"))
		((Vector3r,jointNormal1,Vector3r::Zero(),,"Specifies the normal direction to the joint plane 1. Rk: the ideal here would be to create a vector of vector wich size is defined by the joint integer (as much joint normals as joints). However, it needs to make the pushback function works with python since joint detection is done through a python script. lines 272 to 312 of cpp file should therefore be adapted. [-]"))
		((Vector3r,jointNormal2,Vector3r::Zero(),,"Specifies the normal direction to the joint plane 2. [-]"))
		((Vector3r,jointNormal3,Vector3r::Zero(),,"Specifies the normal direction to the joint plane 3. [-]"))
		,
		createIndex();
	);
	REGISTER_CLASS_INDEX(JCFpmState,State);
};
REGISTER_SERIALIZABLE(JCFpmState);

/** This class holds information associated with each body */
class JCFpmMat: public FrictMat {
  	public:
		virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new JCFpmState); }
		virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<JCFpmState*>(s); }
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(JCFpmMat,FrictMat,"Possibly jointed, cohesive frictional material, for use with other JCFpm classes",
		((int,type,0,,"If particles of two different types interact, it will be with friction only (no cohesion).[-]"))
		((Real,tensileStrength,0.,,"Defines the maximum admissible normal force in traction in the matrix (:yref:`FnMax<JCFpmPhys.FnMax>` = tensileStrength * :yref:`crossSection<JCFpmPhys.crossSection>`). [Pa]"))
		((Real,cohesion,0.,,"Defines the maximum admissible tangential force in shear, for Fn=0, in the matrix (:yref:`FsMax<JCFpmPhys.FsMax>` = cohesion * :yref:`crossSection<JCFpmPhys.crossSection>`). [Pa]"))
                ((Real,residualFrictionAngle,-1.,,"Defines the residual friction angle (when contacts are not cohesive). residualFrictionAngle=frictionAngle if not specified. [degrees]"))
		((Real,jointNormalStiffness,0.,,"Defines the normal stiffness on the joint surface. [Pa/m]"))
		((Real,jointShearStiffness,0.,,"Defines the shear stiffness on the joint surface. [Pa/m]"))
		((Real,jointTensileStrength,0.,,"Defines the :yref:`maximum admissible normal force in traction<JCFpmPhys.FnMax>` on the joint surface. [Pa]"))
		((Real,jointCohesion,0.,,"Defines the :yref:`maximum admissible tangential force in shear<JCFpmPhys.FsMax>`, for Fn=0, on the joint surface. [Pa]"))
		((Real,jointDilationAngle,0,,"Defines the dilatancy of the joint surface (only valid for :yref:`smooth contact logic<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.smoothJoint>`). [rad]"))	
		((Real,jointFrictionAngle,-1,,"Defines Coulomb friction on the joint surface. [rad]"))
		,
		createIndex();
	);
	REGISTER_CLASS_INDEX(JCFpmMat,FrictMat);
};
REGISTER_SERIALIZABLE(JCFpmMat);

/** This class holds information associated with each interaction */
class JCFpmPhys: public NormShearPhys {
	public:
		virtual ~JCFpmPhys();
		
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(JCFpmPhys,NormShearPhys,"Representation of a single interaction of the JCFpm type, storage for relevant parameters",
			((Real,initD,0.,,"equilibrium distance for interacting particles. Computed as the interparticular distance at first contact detection."))
			((bool,isBroken,false,,"flag for broken interactions"))
			((bool,isCohesive,false,,"If false, particles interact in a frictional way. If true, particles are bonded regarding the given :yref:`cohesion<JCFpmMat.cohesion>` and :yref:`tensile strength<JCFpmMat.tensileStrength>` (or their jointed variants)."))
			((bool,more,false,,"specifies if the interaction is crossed by more than 3 joints. If true, interaction is deleted (temporary solution)."))
			((bool,isOnJoint,false,,"defined as true when both interacting particles are :yref:`on joint<JCFpmState.onJoint>` and are in opposite sides of the joint surface. In this case, mechanical parameters of the interaction are derived from the ''joint...'' material properties of the particles. Furthermore, the normal of the interaction may be re-oriented (see :yref:`Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.smoothJoint`)."))
			((bool,isOnSlot,false,,"defined as true when interaction is located in the perforation slot (surface)."))
			((Real,tanFrictionAngle,0.,,"tangent of Coulomb friction angle for this interaction (auto. computed). [-]"))
			((Real,crossSection,0.,,"crossSection=pi*Rmin^2. [m2]"))
			((Real,FnMax,0.,,"positiv value computed from :yref:`tensile strength<JCFpmMat.tensileStrength>` (or joint variant) to define the maximum admissible normal force in traction: Fn >= -FnMax. [N]"))
			((Real,FsMax,0.,,"computed from :yref:`cohesion<JCFpmMat.cohesion>` (or jointCohesion) to define the maximum admissible tangential force in shear, for Fn=0. [N]"))
			((Vector3r,jointNormal,Vector3r::Zero(),,"normal direction to the joint, deduced from e.g. :yref:`<JCFpmState.jointNormal1>`."))
			((Real,jointCumulativeSliding,0.,,"sliding distance for particles interacting on a joint. Used, when :yref:`<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.smoothJoint>` is true, to take into account dilatancy due to shearing. [-]"))
			((Real,tanDilationAngle,0.,,"tangent of the angle defining the dilatancy of the joint surface (auto. computed from :yref:`JCFpmMat.jointDilationAngle`). [-]"))
			((Real,dilation,0.,,"defines the normal displacement in the joint after sliding treshold. [m]"))
			((Real,crackJointAperture,0.,,"Relative displacement between 2 spheres (in case of a crack it is equivalent of the crack aperture)"))
			((bool,breakOccurred,0,,"Flag used to trigger retriangulation as soon as a cohesive bond breaks in FlowEngine (for DFNFlow use only)"))
			((Real,momentEnergy,0,,"reference strain (or kinetic) energy of surrounding interactions (particles)"))
			((Real,momentEnergyChange,0,,"storage of the maximum strain (or kinetic) energy change for surrounding interactions (particles)"))
			((Real,momentMagnitude,0,,"Moment magnitude of a failed interaction"))
			((bool,firstMomentCalc,true,,"Flag for moment calculation"))
			((Real,elapsedIter,0,,"number of elapsed iterations for moment calculation"))
			((bool,momentCalculated,false,,"Flag for moment calculation"))
			((bool,computedCentroid,false,,"Flag for moment calculation"))
			((bool,checkedForCluster,false,,"Have we checked if this int belongs in cluster?"))
			((bool,originalClusterEvent,false,,"the original AE event for a cluster"))
			((bool,clusteredEvent,false,,"is this interaction part of a cluster?"))
			((bool,momentBroken,false,,"Flag for moment calculation"))
			((Real,eventBeginTime,0,,"The time at which event initiated"))			
			((bool,interactionsAdded,false,,"have we added the ints associated with this event?"))
			((int,nearbyFound,0,,"Count used to debug moment calc"))
			((int,eventNumber,0,,"cluster event number"))
			((int,temporalWindow,0,,"temporal window for the clustering algorithm"))
			((Vector3r,momentCentroid,Vector3r::Zero(),,"centroid of the AE event (avg location of clustered breaks)"))
			((vector<Interaction*>,clusterInts,,,"vector of pointers to the broken interactions nearby constituting a cluster"))		
			((Interaction*,originalEvent,,,"pointer to the original interaction of a cluster"))
			((vector<Interaction*>,nearbyInts,,,"vector of pointers to the nearby ints used for moment calc"))
			((Real,strainEnergy,0,,"strain energy of interaction"))
			((Real,kineticEnergy,0,,"kinetic energy of the two spheres participating in the interaction (easiest to store this value with interaction instead of spheres since we are using this information for moment magnitude estimations and associated interaction searches)"))
			,
			createIndex();
			,
		);
		DECLARE_LOGGER;
		REGISTER_CLASS_INDEX(JCFpmPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(JCFpmPhys);

/** 2d functor creating InteractionPhysics (Ip2) taking JCFpmMat and JCFpmMat of 2 bodies, returning type JCFpmPhys */
class Ip2_JCFpmMat_JCFpmMat_JCFpmPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		void distributeCrossSectionsWeibull(shared_ptr<JCFpmPhys> contactPhysics, Real R1, Real R2);
		FUNCTOR2D(JCFpmMat,JCFpmMat);
		DECLARE_LOGGER;
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys,IPhysFunctor,"Converts 2 :yref:`JCFpmMat` instances to one :yref:`JCFpmPhys` instance, with corresponding parameters. See :yref:`JCFpmMat` and [Duriez2016]_ for details",                   
			((int,cohesiveTresholdIteration,1,,"should new contacts be cohesive? If strictly negativ, they will in any case. If positiv, they will before this iter, they won't afterward."))
			((Real,xSectionWeibullShapeParameter,0,,"Shape parameter used to generate interaction radii for the crossSectional areas (changing strength criteria only) according to Weibull distribution. Activated for any value other than 0. Needs to be combined with a :yref:`scale parameter<Ip2_JCFpmMat_JCFpmPhys.xSectionScaleParameter>`)"))
			((Real,xSectionWeibullScaleParameter,1,,"Scale parameter used to generate interaction radii for the crosssectional areas (changing strength criteria only) according to Weibull distribution. Activated for any value other than 0. Needs to be combined with a :yref:`shape parameter<Ip2_JCFpmMat_JCFpmPhys.xSectionShapeParameter>`"))
			((Real,weibullCutOffMin,0.,,"Factor that cuts off the smallest values of the weibull distributed interaction areas."))
			((Real,weibullCutOffMax,10,,"Factor that cuts off the largest values of the weibull distributed interaction areas."))
	);
		
};
REGISTER_SERIALIZABLE(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys);

/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and JCFpmPhys of 2 bodies, returning type JointedCohesiveFrictionalPM */
class Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM: public LawFunctor{
	public:
		virtual bool go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(ScGeom,JCFpmPhys);
		void checkForCluster(JCFpmPhys* phys, ScGeom* geom, Body* b1, Body* b2, Interaction* contact);
		void clusterInteractions(JCFpmPhys* phys, Interaction* contact);
		void computeClusteredMoment(JCFpmPhys* phys);
		void computeCentroid(JCFpmPhys* phys);
		void addUniqueIntsToList(JCFpmPhys* phys, JCFpmPhys* nearbyPhys);
		void computeKineticEnergy(JCFpmPhys* phys, Body* b1, Body* b2);
		void computeTemporalWindow(JCFpmPhys* phys, Body* b1, Body* b2);

		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM,LawFunctor,"Interaction law for cohesive frictional material, e.g. rock, possibly presenting joint surfaces, that can be mechanically described with a smooth contact logic [Ivars2011]_ (implemented in Yade in [Scholtes2012]_). See examples/jointedCohesiveFrictionalPM for script examples. Joint surface definitions (through stl meshes or direct definition with gts module) are illustrated there.",
			((bool,smoothJoint,false,,"if true, interactions of particles belonging to joint surface (:yref:`JCFpmPhys.isOnJoint`) are handled according to a smooth contact logic [Ivars2011]_, [Scholtes2012]_."))
			((bool,neverErase,false,,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene"))
			((bool,cracksFileExist,false,,"if true (and if :yref:`recordCracks<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.recordCracks>`), data are appended to an existing 'cracksKey' text file; otherwise its content is reset."))
			((bool,momentsFileExist,false,,"if true (and if :yref:`recordCracks<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.recordMoments>`), data are appended to an existing 'momentsKey' text file; otherwise its content is reset."))
			((string,Key,"",,"string specifying the name of saved file 'cracks___.txt', when :yref:`recordCracks<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.recordCracks>` is true."))
			((bool,recordCracks,false,,"if true, data about interactions that lose their cohesive feature are stored in the text file cracksKey.txt (see :yref:`Key<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.Key>` and :yref:`cracksFileExist<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.cracksFileExist>`). It contains 9 columns: the break iteration, the 3 coordinates of the contact point, the type (1 means shear break, while 0 corresponds to tensile break), the ''cross section'' (mean radius of the 2 spheres) and the 3 coordinates of the contact normal."))
			((bool,recordMoments,false,,"Combines with :yref: `Key<Law2ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.Key>` to compute acoustic emissions according to clustered broken bond method? (off by default)"))
			((int,nbTensCracks,0,,"number of tensile microcracks."))
			((int,nbShearCracks,0,,"number of shear microcracks."))
			((Real,totalTensCracksE,0.,,"calculate the overall energy dissipated by interparticle microcracking in tension."))
			((Real,totalShearCracksE,0.,,"calculate the overall energy dissipated by interparticle microcracking in shear."))
            ((Real,totalCracksSurface,0.,,"calculate the total cracked surface."))
			((bool,computedCentroid,false,,"computer clustered moments?"))
			((Real,momentRadiusFactor,5.,,"Average particle diameter multiplier for moment magnitude calculation"))
			((int,eventNumber,0,,"cluster event number (used for clustering and paraview visualization of groups)."))
			((Real,momentFudgeFactor,1.,,"Fudge factor used by Hazzard and Damjanac 2013 to improve moment size accuracy (set to 1 for no impact by default)"))
			((bool,clusterMoments,true,,"computer clustered moments? (on by default"))
			((bool,useStrainEnergy,true,,"use strain energy for moment magnitude estimation (if false, use kinetic energy)"))
// 			((bool,recordSlips,false,,"if true, data about frictional interactions that slip are stored in a text file cracksKey.txt."))
// 			((int,nbSlips,0,,"number of slips."))
//			((Real,totalSlipE,0.,,"calculate the overall energy dissipated by interparticle friction."))
		);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);
