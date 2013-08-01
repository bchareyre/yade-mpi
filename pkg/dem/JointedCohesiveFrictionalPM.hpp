/* lucScholtes2010 */

#pragma once

#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/dem/ScGeom.hpp>

/** This class holds information associated with each body state*/
class JCFpmState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(JCFpmState,State,"JCFpm state information about each body.\n\nNone of that is used for computation (at least not now), only for post-processing.",
		((int,tensBreak,0,,"number of tensile breakages. [-]"))
                ((int,shearBreak,0,,"number of shear breakages. [-]"))
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
		((bool,onJoint,false,,"identifies if the particle is on a joint surface. [-]"))
		((int,joint,0,,"indicates the number of joint surfaces to which the particle belongs (0-> no joint, 1->1 joint, etc..). [-]"))
		((Vector3r,jointNormal1,Vector3r::Zero(),,"specifies the normal direction to the joint plane 1. Rk: the ideal here would be to create a vector of vector wich size is defined by the joint integer (as much joint normals as joints). However, it needs to make the pushback function works with python since joint detection is done through a python script. lines 272 to 312 of cpp file should therefore be adapted. [-]"))
		((Vector3r,jointNormal2,Vector3r::Zero(),,"specifies the normal direction to the joint plane 2. [-]"))
		((Vector3r,jointNormal3,Vector3r::Zero(),,"specifies the normal direction to the joint plane 3. [-]")),
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
			((bool,isCohesive,false,,"If false, particles interact in a frictional way. If true, particles are bonded regarding the given cohesion and tensileStrength. [-]"))
			((bool,more,false,,"specifies if the interaction is crossed by more than 3 joints. If true, interaction is deleted (temporary solution). [-]"))
			((bool,isOnJoint,false,,"if true, particles interact as part of a joint (see Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM for details). [-]"))
			((Real,frictionAngle,0,,"defines Coulomb friction. [rad]"))
			((Real,tanFrictionAngle,0,,"tangent of frictionAngle. [-]"))
			((Real,crossSection,0,,"crossSection=pi*Rmin^2. [m2]"))
			((Real,FnMax,0,,"defines the maximum admissible normal force in traction FnMax=tensileStrength*crossSection. [N]"))
			((Real,FsMax,0,,"defines the maximum admissible tangential force in shear FsMax=cohesion*FnMax. [N]"))
			((Vector3r,jointNormal,Vector3r::Zero(),,"Normal direction to the joint."))
			((Real,jointCumulativeSliding,0,,"sliding distance for particles interacting on a joint. Used when soothJoint is true in Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM to take into account dilatancy due to shearing. [-]"))
			((Real,dilationAngle,0,,"defines the dilatancy of the joint surface. [rad]"))
			((Real,tanDilationAngle,0,,"tangent of dilationAngle. [-]"))
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
			((Real,alpha,0.,,"defines the shear to normal stiffness ratio ks/kn."))
			((Real,tensileStrength,0.,,"defines the maximum admissible normal force in traction FnMax=tensileStrength*crossSection in the matrix. [Pa]"))
			((Real,cohesion,0.,,"defines the maximum admissible tangential force in shear FsMax=cohesion*crossSection in the matrix. [Pa]"))
			((Real,jointNormalStiffness,0.,,"defines the normal stiffness on the joint surface. "))
			((Real,jointShearStiffness,0.,,"defines the shear stiffness on the joint surface."))
			((Real,jointTensileStrength,0.,,"defines the maximum admissible normal force in traction FnMax=tensileStrength*crossSection on the joint surface. [Pa]"))
			((Real,jointCohesion,0.,,"defines the maximum admissible tangential force in shear FsMax=cohesion*crossSection on the joint surface. [Pa]"))
			((Real,jointFrictionAngle,-1,,"defines Coulomb friction on the joint surface. [rad]"))
			((Real,jointDilationAngle,0,,"defines the dilatancy of the joint surface (only valid for smooth contact logic). [rad]"))
		);
		
};
REGISTER_SERIALIZABLE(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys);

/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and JCFpmPhys of 2 bodies, returning type JointedCohesiveFrictionalPM */
class Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(ScGeom,JCFpmPhys);

		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM,LawFunctor,"interaction law for jointed frictional material. Basically, this law adds the possibility to define joint surfaces into a cohesive frictional material as defined by :yref:'Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM'. Joint surfaces can be defined in a preprocessing phase through .stl meshes (see ref for details of the procedure).",
			((bool,smoothJoint,false,,"if true, particles belonging to joint surface have a smooth contact logic."))
			((bool,recordCracks,false,,"if true a text file cracks.txt will be created (iteration, position, type (tensile or shear), cross section and contact normal)."))
			((bool,cracksFileExist,false,,"If true, text file already exists and its content won't be reset."))
		);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);
