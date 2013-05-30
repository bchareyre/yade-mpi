/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once

#include <yade/pkg/common/Recorder.hpp>

/*! \brief Record the stress-strain state of a sample in simulations using TriaxialCompressionEngine

	The output is a text file where each line is a record, with the format 
	IterationNumber sigma11 sigma22 sigma33 epsilon11 epsilon22 epsilon33
	
 */


class TriaxialStressController;

class TriaxialStateRecorder : public Recorder
{
	private :
		shared_ptr<TriaxialStressController> triaxialStressController; 
	public :
		virtual ~TriaxialStateRecorder ();
		virtual void action();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(TriaxialStateRecorder,Recorder,"Engine recording triaxial variables (see the variables list in the first line of the output file). This recorder needs :yref:`TriaxialCompressionEngine` or :yref:`ThreeDTriaxialEngine` present in the simulation).",
		((Real,porosity,1,,"porosity of the packing [-]")), //Is it really needed to have this value as a serializable?
		initRun=true;
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TriaxialStateRecorder);
