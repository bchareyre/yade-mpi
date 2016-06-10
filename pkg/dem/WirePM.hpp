/*************************************************************************
*  Copyright (C) 2010 by Klaus Thoeni                                    *
*  klaus.thoeni@newcastle.edu.au                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/**
=== OVERVIEW OF WirePM ===

A particle model to simulate single wires and rockfall meshes (see Bertrad et al. 2005, Bertrad et al. 2008, Thoeni et al. 2013).

Features of the interaction law:

1. The law is designed for particles which do not touch. A link will be created by defining an interaction radius and running the threshold iteration.

2. The contact law is for tension only. Compressive forces never exist between the particles. However, elastic unloading is considered.

3. The force displacement curve which defines the interaction forces is piecewise linear and defined by the stress-strain curve of the wire material. Any piecewise linear curve can be used. 

4. Three different types of wire models are available.

*/

#pragma once

#include<pkg/common/ElastMat.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/dem/ScGeom.hpp>

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
class WireMat: public FrictMat {
	public:
		virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new WireState); }
		virtual bool stateTypeOk(State* s) const { return (bool)dynamic_cast<WireState*>(s); }
		void postLoad(WireMat&);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(WireMat,FrictMat,"Material for use with the Wire classes. In conjunction with the corresponding functors it can be used to model steel wire meshes [Thoeni2014]_, geotextiles [Cheng2016]_ and more.",
			((Real,diameter,0.0027,,"Diameter of the single wire in [m] (the diameter is used to compute the cross-section area of the wire)."))
			((unsigned int,type,0,,"Three different types are considered:\n\n"
			"== ===============================================================\n"
			"0  Corresponds to Bertrand's approach (see [Bertrand2008]_): only one stress-strain curve is used\n"
			"1  New approach: two separate stress-strain curves can be used (see [Thoeni2013]_)\n"
			"2  New approach with stochastically distorted contact model: two separate stress-strain curves with changed initial stiffness and horizontal shift (shift is random if $\\text{seed}\\geq0$, for more details see [Thoeni2013]_)\n"
			"== ===============================================================\n\n"
			"By default the type is 0."
			))
			((vector<Vector2r>,strainStressValues,,Attr::triggerPostLoad,"Piecewise linear definition of the stress-strain curve by set of points (strain[-]>0,stress[Pa]>0) for one single wire. Tension only is considered and the point (0,0) is not needed! NOTE: Vector needs to be initialized!"))
			((vector<Vector2r>,strainStressValuesDT,,Attr::triggerPostLoad,"Piecewise linear definition of the stress-strain curve by set of points (strain[-]>0,stress[Pa]>0) for the double twist. Tension only is considered and the point (0,0) is not needed! If this value is given the calculation will be based on two different stress-strain curves without considering the parameter introduced by [Bertrand2008]_ (see [Thoeni2013]_)."))
			((bool,isDoubleTwist,false,,"Type of the mesh. If true two particles of the same material which body ids differ by one will be considered as double-twisted interaction."))
			((Real,lambdaEps,0.47,,"Parameter between 0 and 1 to reduce strain at failure of a double-twisted wire (as used by [Bertrand2008]_). [-]"))
			((Real,lambdak,0.73,,"Parameter between 0 and 1 to compute the elastic stiffness of a double-twisted wire (as used by [Bertrand2008]_): $k^D=2(\\lambda_k k_h + (1-\\lambda_k)k^S)$. [-]"))
			((int,seed,12345,,"Integer used to initialize the random number generator for the calculation of the distortion. If the integer is equal to 0 a internal seed number based on the time is computed. [-]"))
			((Real,lambdau,0.2,,"Parameter between 0 and 1 introduced by [Thoeni2013]_ which defines the maximum shift of the force-displacement curve in order to take an additional initial elongation (e.g. wire distortion/imperfections, slipping, system flexibility) into account: $\\Delta l^*=\\lambda_u l_0 \\text{rnd(seed)}$. [-]"))
			((Real,lambdaF,1.0,,"Parameter between 0 and 1 introduced by [Thoeni2013]_ which defines where the shifted force-displacement curve intersects with the new initial stiffness: $F^*=\\lambda_F F_{\\text{elastic}}$. [-]"))
			((Real,as,0.,Attr::readonly,"Cross-section area of a single wire used to transform stress into force. [m²]"))
		,
		createIndex();
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(WireMat,FrictMat);
};
REGISTER_SERIALIZABLE(WireMat);

/** This class holds information associated with each interaction */
// NOTE: even if WirePhys has no shear force it is derived from FrictPhys since all implemented functions (e.g. unbalancedForce) work properly for FrictPhys only
class WirePhys: public FrictPhys {
	public:
		virtual ~WirePhys();
	
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(WirePhys,FrictPhys,"Representation of a single interaction of the WirePM type, storage for relevant parameters",
			((Real,initD,0.,,"Equilibrium distance for particles. Computed as the initial inter-particular distance when particle are linked."))
			((bool,isLinked,false,,"If true particles are linked and will interact. Interactions are linked automatically by the definition of the corresponding interaction radius. The value is false if the wire breaks (no more interaction)."))
			((bool,isDoubleTwist,false,,"If true the properties of the interaction will be defined as a double-twisted wire."))
			((vector<Vector2r>,displForceValues,,Attr::readonly,"Defines the values for force-displacement curve."))
			((vector<Real>,stiffnessValues,,Attr::readonly,"Defines the values for the various stiffnesses (the elastic stiffness is stored as kn)."))
			((Real,plastD,0.,Attr::readonly,"Plastic part of the inter-particular distance of the previous step. \n\n.. note::\n\t Only elastic displacements are reversible (the elastic stiffness is used for unloading) and compressive forces are inadmissible. The compressive stiffness is assumed to be equal to zero.\n\n.."))
			((Real,limitFactor,0.,Attr::readonly,"This value indicates on how far from failing the wire is, e.g. actual normal displacement divided by admissible normal displacement."))
			((bool,isShifted,false,Attr::readonly,"If true :yref:`WireMat` type=2 and the force-displacement curve will be shifted."))
			((Real,dL,0.,Attr::readonly,"Additional wire length for considering the distortion for :yref:`WireMat` type=2 (see [Thoeni2013]_)."))
			,
			createIndex();
			,
		);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(WirePhys,FrictPhys);
};
REGISTER_SERIALIZABLE(WirePhys);

/** 2d functor creating IPhys (Ip2) taking WireMat and WireMat of 2 bodies, returning type WirePhys */
class Ip2_WireMat_WireMat_WirePhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		
		FUNCTOR2D(WireMat,WireMat);
		
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_WireMat_WireMat_WirePhys,IPhysFunctor,"Converts 2 :yref:`WireMat` instances to :yref:`WirePhys` with corresponding parameters.",
			((int,linkThresholdIteration,1,,"Iteration to create the link."))
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_WireMat_WireMat_WirePhys);

/** 2d functor creating the interaction law (Law2) based on SphereContactGeometry (ScGeom) and WirePhys of 2 bodies, returning type WirePM */
class Law2_ScGeom_WirePhys_WirePM: public LawFunctor{
	public:
		virtual bool go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		
		FUNCTOR2D(ScGeom,WirePhys);

		YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_WirePhys_WirePM,LawFunctor,"Constitutive law for the wire model.",
			((int,linkThresholdIteration,1,,"Iteration to create the link."))
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_WirePhys_WirePM);
