/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/common/ElastMat.hpp>
#include<pkg/common/Dispatching.hpp>

class CapillaryPhys : public FrictPhys
{
	public :
		int currentIndexes [4]; // used for faster interpolation (stores previous positions in tables)
		
		virtual ~CapillaryPhys() {};

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(CapillaryPhys,FrictPhys,"Physics (of interaction) for :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`.",
				 ((bool,meniscus,false,Attr::readonly,"True when a meniscus with a non-zero liquid volume (:yref:`vMeniscus<CapillaryPhys.vMeniscus>`) has been computed for this interaction"))
				 ((bool,isBroken,false,,"Might be set to true by the user to make liquid bridge inactive (capillary force is zero)"))
				 ((Real,capillaryPressure,0.,,"Value of the capillary pressure Uc. Defined as Ugas-Uliquid, obtained from :yref:`corresponding Law2 parameter<Law2_ScGeom_CapillaryPhys_Capillarity.capillaryPressure>`"))
				 ((Real,vMeniscus,0.,,"Volume of the meniscus"))
				 ((Real,Delta1,0.,,"Defines the surface area wetted by the meniscus on the smallest grains of radius R1 (R1<R2)"))
				 ((Real,Delta2,0.,,"Defines the surface area wetted by the meniscus on the biggest grains of radius R2 (R1<R2)"))
				 ((Vector3r,fCap,Vector3r::Zero(),,"Capillary force produced by the presence of the meniscus. This is the force acting on particle #2"))
				 ((short int,fusionNumber,0.,,"Indicates the number of meniscii that overlap with this one"))
				 ((Real,nn11,0.,,":math:`\\iint_A n_1 n_1 \\, dS = \\iint_A n_2 n_2 \\, dS`, $A$ being the liquid-gas surface of the meniscus, $\\vec n$ the associated normal, and $(1,2,3)$ a local basis with $3$ the meniscus orientation (:yref:`ScGeom.normal`). NB: $A$ = 2 :yref:`nn11<CapillaryPhys.nn11>` + :yref:`nn33<CapillaryPhys.nn33>`."))
				 ((Real,nn33,0.,,":math:`\\iint_A n_3 n_3 \\, dS`, $A$ being the liquid-gas surface of the meniscus, $\\vec n$ the associated normal, and $(1,2,3)$ a local basis with $3$ the meniscus orientation (:yref:`ScGeom.normal`). NB: $A$ = 2 :yref:`nn11<CapillaryPhys.nn11>` + :yref:`nn33<CapillaryPhys.nn33>`."))
				 ,,
				 createIndex();currentIndexes[0]=currentIndexes[1]=currentIndexes[2]=currentIndexes[3]=0;
				 ,
				 );
	REGISTER_CLASS_INDEX(CapillaryPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(CapillaryPhys);


class Ip2_FrictMat_FrictMat_CapillaryPhys : public IPhysFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_FrictMat_FrictMat_CapillaryPhys,IPhysFunctor, "RelationShips to use with :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`.\n\n In these RelationShips all the interaction attributes are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<IPhysFunctor>`, most of the attributes are computed only once, when the interaction is new.",,;);
	
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_CapillaryPhys);

