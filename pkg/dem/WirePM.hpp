/* Klaus Thoeni 2010 */

/**
=== OVERVIEW OF WirePM ===

A particle model to simulate single wires and rockfall meshes (see Bertrad et al. 2005, Bertrad et al. 2008).

Features of the interaction law:

1. The law is designed for particles which do not touch. A link will be created by defining an interaction radius and running the threshold iteration.

2. The contact law is for tension only. Compressive forces never exist between the particles. However, elastic unloading is considered.

3. The force displacement curve which defines the interaction forces is piecewise linear and defined by the stress-strain curve of the wire material. Any piecewise linear curve can be used. 

Remarks:
- The contact law is new and still needs some testing :-) 
*/

#pragma once

#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/NormShearPhys.hpp>
#include<yade/pkg/dem/ScGeom.hpp>

/** This class holds information associated with each body state*/
class WireState: public State {
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(WireState,State,"Wire state information of each body.\n\nNone of that is used for computation (at least not now), only for post-processing.",
		((int,numBrokenLinks,0,,"Number of broken links (e.g. number of wires connected to the body which are broken). [-]"))
		,
		createIndex();
	);
	REGISTER_CLASS_INDEX(WireState,State);
};
REGISTER_SERIALIZABLE(WireState);

/** This class holds information associated with each body */
class WireMat: public ElastMat {
	public:
		virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new WireState); }
		virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<WireState*>(s); }
		void postLoad(WireMat&);
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(WireMat,ElastMat,"Material for use with the Wire classes",
		((Real,diameter,0.0027,," (Diameter of the single wire in [m] (the diameter is used to compute the cross-section area of the wire)."))
		((vector<Vector2r>,strainStressValues,,Attr::triggerPostLoad,"Piecewise linear definition of the stress-strain curve by set of points (strain[-]>0,stress[Pa]>0) for one single wire. Tension only is considered and the point (0,0) is not needed!"))
		((bool,isDoubleTwist,false,,"Type of the mesh. If true two particles of the same material which body ids differ by one will be considered as double-twisted interaction."))
		((Real,lambdaEps,0.4,,"Parameter between 0 and 1 to reduce the failure strain of the double-twisted wire (as used by [Bertrand2008]_). [-]"))
		((Real,lambdak,0.21,,"Parameter between 0 and 1 to compute the elastic stiffness of the double-twisted wire (as used by [Bertrand2008]_): $k^D=2(\\lambda_k k_h + (1-\\lambda_k)k^S)$. [-]"))
		((Real,as,0,Attr::readonly,"Cross-section area of a single wire used for the computation of the limit normal contact forces. [m²]"))
		,
		createIndex();
	);
	REGISTER_CLASS_INDEX(WireMat,ElastMat);
};
REGISTER_SERIALIZABLE(WireMat);

/** This class holds information associated with each interaction */
class WirePhys: public NormPhys {
	public:
		virtual ~WirePhys();
	
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(WirePhys,NormPhys,"Representation of a single interaction of the WirePM type, storage for relevant parameters",
			((Real,initD,0,,"Equilibrium distance for particles. Computed as the initial inter-particular distance when particle are linked."))
			((bool,isLinked,false,,"If true particles are linked and will interact. Interactions are linked automatically by the definition of the corresponding interaction radius. The value is false if the wire breaks (no more interaction)."))
			((bool,isDoubleTwist,false,,"If true the properties of the interaction will be defined as a double-twisted wire."))
			((vector<Vector2r>,displForceValues,,(Attr::readonly|Attr::noSave),"Defines the values for force-displacement curve."))
			((vector<Real>,stiffnessValues,,(Attr::readonly|Attr::noSave),"Defines the values for the different stiffness (first value corresponds to elastic stiffness kn)."))
			((Real,plastD,0,Attr::readonly,"Plastic part of the inter-particular distance of the previous step. \n\n.. note::\n\t Only elastic displacements are reversible (the elastic stiffness is used for unloading) and compressive forces are inadmissible. The compressive stiffness is assumed to be equal to zero (see [Bertrand2005]_).\n\n.."))
			,
			createIndex();
			,
		);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(WirePhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(WirePhys);

/** 2d functor creating IPhys (Ip2) taking WireMat and WireMat of 2 bodies, returning type WirePhys */
class Ip2_WireMat_WireMat_WirePhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		
		FUNCTOR2D(WireMat,WireMat);
		DECLARE_LOGGER;
		
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_WireMat_WireMat_WirePhys,IPhysFunctor,"Converts 2 :yref:`WireMat` instances to :yref:`WirePhys` with corresponding parameters.",
			((int,linkThresholdIteration,1,,"Iteration to create the link."))
		);
};
REGISTER_SERIALIZABLE(Ip2_WireMat_WireMat_WirePhys);

/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and WirePhys of 2 bodies, returning type WirePM */
class Law2_ScGeom_WirePhys_WirePM: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		
		FUNCTOR2D(ScGeom,WirePhys);

		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_WirePhys_WirePM,LawFunctor,"Constitutive law for the wire model.",
			((int,linkThresholdIteration,1,,"Iteration to create the link."))
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_WirePhys_WirePM);
