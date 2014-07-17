/*************************************************************************
*  Copyright (C) 2010 by Jerome Duriez <jerome.duriez@hmg.inpg.fr>       *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg/common/ElastMat.hpp>
#include <yade/pkg/dem/FrictPhys.hpp>
#include <yade/pkg/common/Dispatching.hpp>
#include <yade/pkg/dem/ScGeom.hpp>

class NormalInelasticMat : public FrictMat
{
	public :
		virtual ~NormalInelasticMat () {};

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(NormalInelasticMat,FrictMat,"Material class for particles whose contact obey to a normal inelasticity (governed by this :yref:`coeff_dech<NormalInelasticMat::coeff_dech>`).",
		((Real,coeff_dech,1.0,,"=kn(unload) / kn(load)"))
		,
		createIndex();
					);
/// Indexable
	REGISTER_CLASS_INDEX(NormalInelasticMat,FrictMat);
};

REGISTER_SERIALIZABLE(NormalInelasticMat);


class NormalInelasticityPhys : public FrictPhys
{
	public :
		virtual ~NormalInelasticityPhys() {};

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(NormalInelasticityPhys,FrictPhys,
				"Physics (of interaction) for using :yref:`Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity` : with inelastic unloadings",
				((Real,unMax,0.0,,"the maximum value of penetration depth of the history of this interaction"))
				((Real,previousun,0.0,,"the value of this un at the last time step"))
				((Real,previousFn,0.0,,"the value of the normal force at the last time step"))
				((Real,forMaxMoment,1.0,,"parameter stored for each interaction, and allowing to compute the maximum value of the exchanged torque : TorqueMax= forMaxMoment * NormalForce"))
				((Real,kr,0.0,,"the rolling stiffness of the interaction"))
				((Real,knLower,0.0,,"the stifness corresponding to a virgin load for example"))
				// internal attributes
				((Vector3r,moment_twist,Vector3r(0,0,0),(Attr::noSave | Attr::readonly),"Twist moment. Defined here, being initialized as it should be, to be used in :yref:`Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity`"))
				((Vector3r,moment_bending,Vector3r(0,0,0),(Attr::noSave | Attr::readonly),"Bending moment. Defined here, being initialized as it should be, to be used in :yref:`Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity`"))
				,
				createIndex();
				);
	REGISTER_CLASS_INDEX(NormalInelasticityPhys,FrictPhys);
};

REGISTER_SERIALIZABLE(NormalInelasticityPhys);


class Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity : public LawFunctor
{
	private :
		Vector3r moment // the part of the contact torque of the interaction due to relative rotations (a first part is due to contact forces)
			,f// contact force
			;
		Real Fn	 // value of normal force in the interaction
		    ,Fs // shear force
		    ,maxFs; // maximum value of shear force according to Coulomb-like criterion
		Real un;	 // value of interpenetration in the interaction
	public :
		virtual void go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);

	FUNCTOR2D(ScGeom,NormalInelasticityPhys);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity,
				LawFunctor,
				"Contact law used to simulate granular filler in rock joints [Duriez2009a]_, [Duriez2011]_. It includes possibility of cohesion, moment transfer and inelastic compression behaviour (to reproduce the normal inelasticity observed for rock joints, for the latter).\n\n The moment transfer relation corresponds to the adaptation of the work of Plassiard & Belheine (see in [DeghmReport2006]_ for example), which was realized by J. Kozicki, and is now coded in :yref:`ScGeom6D`.\n\n As others :yref:`LawFunctor`, it uses pre-computed data of the interactions (rigidities, friction angles -with their tan()-, orientations of the interactions); this work is done here in :yref:`Ip2_2xNormalInelasticMat_NormalInelasticityPhys`.\n\n To use this you should also use :yref:`NormalInelasticMat` as material type of the bodies.\n\n The effects of this law are illustrated in examples/normalInelasticity-test.py",
				((bool,momentRotationLaw,true,,"boolean, true=> computation of a torque (against relative rotation) exchanged between particles"))
				((bool,momentAlwaysElastic,false,,"boolean, true=> the part of the contact torque (caused by relative rotations, which is computed only if momentRotationLaw..) is not limited by a plastic threshold"))
				,
				moment=Vector3r::Zero();
				f=Vector3r::Zero();
				Fn=0.0;
				Fs=0.0;
				maxFs=0.0;
				un=0.0;
				);
	
};

REGISTER_SERIALIZABLE(Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity);

/*! \brief The RelationShips for using Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity

In these RelationShips all the attributes of the interactions (which are of NormalInelasticityPhys type) are computed.
WARNING : as in the others Relationships most of the attributes are computed only once : when the interaction is "new"
 */

class Ip2_2xNormalInelasticMat_NormalInelasticityPhys : public IPhysFunctor
{
	public :

		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
		
	FUNCTOR2D(NormalInelasticMat,NormalInelasticMat);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_2xNormalInelasticMat_NormalInelasticityPhys,
				  IPhysFunctor,
				  "The RelationShips for using :yref:`Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity`\n\n In these RelationShips all the attributes of the interactions (which are of NormalInelasticityPhys type) are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<IPhysFunctor>`, most of the attributes are computed only once, when the interaction is new.",
				  ((Real,betaR,0.12,,"Parameter for computing the torque-stifness : T-stifness = betaR * Rmoy^2"))
				  );
};

REGISTER_SERIALIZABLE(Ip2_2xNormalInelasticMat_NormalInelasticityPhys);

