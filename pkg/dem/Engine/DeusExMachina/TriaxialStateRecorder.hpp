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

#include <yade/pkg-common/Recorder.hpp>

/*! \brief Record the stress-strain state of a sample in simulations using TriaxialCompressionEngine

	The output is a text file where each line is a record, with the format 
	IterationNumber sigma11 sigma22 sigma33 epsilon11 epsilon22 epsilon33
	
 */


class TriaxialCompressionEngine;

class TriaxialStateRecorder : public Recorder
{
	private :
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		bool changed;
	public :
		Real 		porosity;
		
		//Real height, width, depth;
		//Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		
		//int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		TriaxialStateRecorder ();
		virtual void action(World*);
	DECLARE_LOGGER;
	REGISTER_ATTRIBUTES(Recorder,(porosity));

	protected :
	REGISTER_CLASS_AND_BASE(TriaxialStateRecorder,Recorder);
};
REGISTER_SERIALIZABLE(TriaxialStateRecorder);


