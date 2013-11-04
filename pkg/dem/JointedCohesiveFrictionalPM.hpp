/* lucScholtes2010 */

#pragma once

#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/dem/ScGeom.hpp>

/** This class holds information associated with each body state*/
class JCFpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(JCFpmState,State,"JCFpm state information about each body.",
		((int,tensBreak,0,,"number of tensile breakages. [-]"))
                ((int,shearBreak,0,,"number of shear breakages. [-]"))
		((int,noIniLinks,0,,"number of initial cohesive interactions. [-]"))
		((Real,tensBreakRel,0,,"relative number (in [0;1]) of tensile breakages (compared with noIniLinks). [-]"))
		((Real,shearBreakRel,0,,"relative number (in [0;1]) of shear breakages (compared with noIniLinks). [-]"))
		((bool,onJoint,false,,"identifies if the particle is on a joint surface."))
		((int,joint,0,,"indicates the number of joint surfaces to which the particle belongs (0-> no joint, 1->1 joint, etc..). [-]"))
		((Vector3r,jointNormal1,Vector3r::Zero(),,"specifies the normal direction to the joint plane 1. Rk: the ideal here would be to create a vector of vector wich size is defined by the joint integer (as much joint normals as joints). However, it needs to make the pushback function works with python since joint detection is done through a python script. lines 272 to 312 of cpp file should therefore be adapted. [-]"))
		((Vector3r,jointNormal2,Vector3r::Zero(),,"specifies the normal direction to the joint plane 2. [-]"))
		((Vector3r,jointNormal3,Vector3r::Zero(),,"specifies the normal direction to the joint plane 3. [-]"))
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
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(JCFpmMat,FrictMat,"possibly jointed cohesive frictional material, for use with other JCFpm classes",
		((int,type,0,,"if particles of two different types interact, it will be with friction only (no cohesion).[-]"))
		//((Real,alpha,0.,,"defines the shear to normal stiffness ratio ks/kn in the matrix."))
		((Real,tensileStrength,0.,,"defines the maximum admissible normal force in traction in the matrix (:yref:`JCFpmPhys.FnMax`=tensileStrength*:yref:`JCFpmPhys.crossSection`). [Pa]"))
		((Real,cohesion,0.,,"defines the maximum admissible tangential force in shear, for Fn=0, in the matrix (:yref:`JCFpmPhys.FsMax`=cohesion*:yref:`JCFpmPhys.crossSection`). [Pa]"))
		((Real,jointNormalStiffness,0.,,"defines the normal stiffness on the joint surface. [Pa/m]"))
		((Real,jointShearStiffness,0.,,"defines the shear stiffness on the joint surface. [Pa/m]"))
		((Real,jointTensileStrength,0.,,"defines the :yref:`maximum admissible normal force in traction<JCFpmPhys.FnMax>` on the joint surface. [Pa]"))
		((Real,jointCohesion,0.,,"defines the :yref:`maximum admissible tangential force in shear<JCFpmPhys.FsMax>`, for Fn=0, on the joint surface. [Pa]"))
		((Real,jointFrictionAngle,-1,,"defines Coulomb friction on the joint surface. [rad]"))
		((Real,jointDilationAngle,0,,"defines the dilatancy of the joint surface (only valid for :yref:`smooth contact logic<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.smoothJoint`). [rad]"))	
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

		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(JCFpmPhys,NormShearPhys,"representation of a single interaction of the JCFpm type, storage for relevant parameters",
			((Real,initD,0,,"equilibrium distance for interacting particles. Computed as the interparticular distance at first contact detection."))
			((bool,isCohesive,false,,"If false, particles interact in a frictional way. If true, particles are bonded regarding the given :yref:`cohesion<JCFpmMat.cohesion>` and :yref:`tensile strength<JCFpmMat.tensileStrength>` (or their jointed variants)."))
			((bool,more,false,,"specifies if the interaction is crossed by more than 3 joints. If true, interaction is deleted (temporary solution)."))
			((bool,isOnJoint,false,,"defined as true when both interacting particles are :yref:`on joint<JCFpmState.onJoint>` and are in opposite sides of the joint surface. In this case, mechanical parameters of the interaction are derived from the ''joint...'' material properties of the particles, and normal of the interaction is re-oriented (see also :yref:`Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM`)."))
			((Real,tanFrictionAngle,0,,"tangent of Coulomb friction angle for this interaction (auto. computed). [-]"))
			((Real,crossSection,0,,"crossSection=pi*Rmin^2. [m2]"))
			((Real,FnMax,0,,"computed from :yref:`tensile strength<JCFpmMat.tensileStrength>` (or joint variant) to define the maximum admissible normal force in traction. [N]"))
			((Real,FsMax,0,,"computed from :yref:`cohesion<JCFpmMat.cohesion>` (or jointCohesion) to define the maximum admissible tangential force in shear, for Fn=0. [N]"))
			((Vector3r,jointNormal,Vector3r::Zero(),,"normal direction to the joint, deduced from e.g. <JCFpmState.jointNormal1>."))
			((Real,jointCumulativeSliding,0,,"sliding distance for particles interacting on a joint. Used, when :yref:`<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.smoothJoint>` is true, to take into account dilatancy due to shearing. [-]"))
			//((Real,dilationAngle,0,,"defines the dilatancy of the joint surface. [rad]"))
			((Real,tanDilationAngle,0,,"tangent of the angle defining the dilatancy of the joint surface (auto. computed from :yref:`JCFpmMat.jointDilationAngle`). [-]"))
			((Real,dilation,0,,"defines the normal displacement in the joint after sliding treshold. [m]"))
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
		
		FUNCTOR2D(JCFpmMat,JCFpmMat);
		DECLARE_LOGGER;
		
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys,IPhysFunctor,"converts 2 JCFpmMat instances to JCFpmPhys with corresponding parameters.",
			((int,cohesiveTresholdIteration,1,,"should new contacts be cohesive? They will before this iter, they won't afterward."))
		);
		
};
REGISTER_SERIALIZABLE(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys);

/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and JCFpmPhys of 2 bodies, returning type JointedCohesiveFrictionalPM */
class Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(ScGeom,JCFpmPhys);

		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM,LawFunctor,"Interaction law for cohesive frictional material, e.g. rock, possibly presenting joint surfaces. Joint surfaces can be defined in a preprocessing phase through .stl meshes (see ref for details of the procedure), and can be mechanically described with a smooth contact logic [Ivars2011]_ (implemented in Yade in [Scholtes2012]_).",
			((bool,smoothJoint,false,,"if true, interactions of particles belonging to joint surface (:yref:`JCFpmPhys.isOnJoint`) are handled according to a smooth contact logic [Ivars2011]_, [Scholtes2012]_."))
			((bool,recordCracks,false,,"if true a text file cracksKey.txt (see below) will be created (apparition iteration, position, type (tensile or shear), cross section and contact normal)."))
			((string,Key,"",,"string specifying the name of saved file 'cracks___.txt', when :yref:`recordCracks<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.`recordCracks>` is true."))
			((bool,cracksFileExist,false,,"if true (and if :yref:`recordCracks<Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM.`recordCracks>`), data are appended to an existing 'cracks...' text file; otherwise its content is reset."))
		);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);
