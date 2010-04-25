/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/pkg-dem/FrictPhys.hpp>

class CapillaryPhys : public FrictPhys
{
	public :
		int currentIndexes [4]; // used for faster interpolation (stores previous positions in tables)
		
		virtual ~CapillaryPhys();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CapillaryPhys,FrictPhys,"Physics (of interaction) for Law2_ScGeom_CapillaryPhys_Capillarity. Rk: deprecated -> needs some work to be conform with the new formalism!",
				 ((bool,meniscus,false,"Presence of a meniscus if true"))
				 ((Real,CapillaryPressure,0.,"Value of the capillary pressure Uc defines as Ugas-Uliquid"))
				 ((Real,Vmeniscus,0.,"Volume of the menicus"))
				 ((Real,Delta1,0.,"Defines the surface area wetted by the meniscus on the smallest grains of radius R1 (R1<R2)"))
				 ((Real,Delta2,0.,"Defines the surface area wetted by the meniscus on the biggest grains of radius R2 (R1<R2)"))
				 ((Vector3r,Fcap,Vector3r::Zero(),"Capillary Force produces by the presence of the meniscus"))
				 ((short int,fusionNumber,0.,"Indicates the number of meniscii that overlap with this one"))
				 ,createIndex();currentIndexes[0]=currentIndexes[1]=currentIndexes[2]=currentIndexes[3]=0;
				 );
	REGISTER_CLASS_INDEX(CapillaryPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(CapillaryPhys);




