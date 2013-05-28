/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/dem/KinemSimpleShearBox.hpp>


class KinemCTDEngine : public KinemSimpleShearBox
{
	private :
		Real current_sigma		// Computed in kPa
			;

		int		temoin;

	public :
		virtual ~KinemCTDEngine();

		void action();
// 			;

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCTDEngine,KinemSimpleShearBox,
			"To compress a simple shear sample by moving the upper box in a vertical way only, so that the tangential displacement (defined by the horizontal gap between the upper and lower boxes) remains constant (thus, the CTD = Constant Tangential Displacement).\n \t The lateral boxes move also to keep always contact. All that until this box is submitted to a given stress (=*targetSigma*). Moreover saves are executed at each value of stresses stored in the vector *sigma_save*, and at *targetSigma*",
			((Real,compSpeed,0.0,,"(vertical) speed of the upper box : >0 for real compression, <0 for unloading [$m/s$]"))
			((std::vector<Real>,sigma_save,,,"vector with the values of sigma at which a save of the simulation should be performed [$kPa$]"))
			((Real,targetSigma,0.0,,"the value of sigma at which the compression should stop [$kPa$]")),
			temoin=0;
						)
	

};

REGISTER_SERIALIZABLE(KinemCTDEngine);


