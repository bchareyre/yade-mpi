/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/core/Body.hpp>
#include<Wm3Vector3.h>


class KinemCTDEngine : public PartialEngine
{
	private :
		Real	alpha	// angle from the lower plate to the left box (trigo wise), the Engine finds itself its value
			,dalpha	// the increment over alpha to set, to cancel the offset due to vertical displacement of upper box
			,current_sigma		// Computed in kPa
			;
		void stopMovement();		// to cancel all the (rotation) velocities when Flim is reached

		int		temoin;

		shared_ptr<Body> leftbox;
		shared_ptr<Body> rightbox;
		shared_ptr<Body> frontbox;
		shared_ptr<Body> backbox;
		shared_ptr<Body> boxbas;
		shared_ptr<Body> topbox;

		void letMove();

	public :
		virtual ~KinemCTDEngine();

		void action()
			,computeAlpha()
			;

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(KinemCTDEngine,PartialEngine,
			"To compress a simple shear sample by moving the upper box in a vertical way only, so that the tangential displacement (defined by the horizontal gap between the upper and lower boxes) remains constant.\n \t The lateral boxes move also to keep always contact. All that until this box is submitted to a given stress (=*target_sigma*). Moreover saves are executed at each value of stresses stored in the vector *sigma_save*, and at *target_sigma*",
			((Real,compSpeed,0.0,"(vertical) speed of the upper box : >0 for real compression, <0 for unloading [m/s]"))
			((std::vector<Real>,sigma_save,,"vector with the values of sigma at which a save of the simulation should be performed [kPa]"))
			((std::vector<Real>,temoin_save,,"vector (same length as 'sigma_save'), with 0 or 1 depending whether the save for the corresponding value of gamma has been done (1) or not (0)"))
			((Real,target_sigma,0.0,"the value of sigma at which the compression should stop [kPa]"))
			((body_id_t,id_topbox,3,"the id of the upper wall"))
			((body_id_t,id_boxbas,1,"the id of the lower wall"))
			((body_id_t,id_boxleft,0,"the id of the left wall"))
			((body_id_t,id_boxright,2,"the id of the right wall"))
			((body_id_t,id_boxfront,5,"the id of the wall in front of the sample"))
			((body_id_t,id_boxback,4,"the id of the wall at the back of the sample"))
			((string,Key,"","string to add at the names of the saved files")),
			temoin=0;
						)
	

};

REGISTER_SERIALIZABLE(KinemCTDEngine);


