/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/dem/HertzMindlin.hpp>

class MindlinCapillaryPhys : public MindlinPhys
{
	public :
		int currentIndexes [4]; // used for faster interpolation (stores previous positions in tables)
		
		virtual ~MindlinCapillaryPhys();

	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(MindlinCapillaryPhys,MindlinPhys,"Adds capillary physics to Mindlin's interaction physics.",
				((bool,meniscus,false,,"Presence of a meniscus if true"))
				((bool,isBroken,false,,"If true, capillary force is zero and liquid bridge is inactive."))
				((Real,capillaryPressure,0.,,"Value of the capillary pressure Uc defines as Ugas-Uliquid"))
				((Real,vMeniscus,0.,,"Volume of the menicus"))
				((Real,Delta1,0.,,"Defines the surface area wetted by the meniscus on the smallest grains of radius R1 (R1<R2)"))
				((Real,Delta2,0.,,"Defines the surface area wetted by the meniscus on the biggest grains of radius R2 (R1<R2)"))
				((Vector3r,fCap,Vector3r::Zero(),,"Capillary Force produces by the presence of the meniscus"))
				((short int,fusionNumber,0.,,"Indicates the number of meniscii that overlap with this one"))
				,/*deprec*/
				((Fcap,fCap,"naming convention"))
				((CapillaryPressure,capillaryPressure,"naming convention"))
				,,createIndex();currentIndexes[0]=currentIndexes[1]=currentIndexes[2]=currentIndexes[3]=0;
				,
				);
	REGISTER_CLASS_INDEX(MindlinCapillaryPhys,MindlinPhys);
};
REGISTER_SERIALIZABLE(MindlinCapillaryPhys);


class Ip2_FrictMat_FrictMat_MindlinCapillaryPhys : public IPhysFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_FrictMat_FrictMat_MindlinCapillaryPhys,IPhysFunctor, "RelationShips to use with Law2_ScGeom_CapillaryPhys_Capillarity\n\n In these RelationShips all the interaction attributes are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<IPhysFunctor>`, most of the attributes are computed only once, when the interaction is new.",
	            ((Real,gamma,0.0,,"Surface energy parameter [J/m^2] per each unit contact surface, to derive DMT formulation from HM"))
				((Real,eta,0.0,,"Coefficient to determine the plastic bending moment"))
				((Real,krot,0.0,,"Rotational stiffness for moment contact law"))
				((Real,ktwist,0.0,,"Torsional stiffness for moment contact law"))
				((shared_ptr<MatchMaker>,en,,,"Normal coefficient of restitution $e_n$."))
				((shared_ptr<MatchMaker>,es,,,"Shear coefficient of restitution $e_s$."))
				((shared_ptr<MatchMaker>,betan,,,"Normal viscous damping coefficient $\\beta_n$."))
				((shared_ptr<MatchMaker>,betas,,,"Shear viscous damping coefficient $\\beta_s$."))
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_MindlinCapillaryPhys);



