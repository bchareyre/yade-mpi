/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TRIAXIAL_STATE_RECORDER_HPP
#define TRIAXIAL_STATE_RECORDER_HPP

#include <yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

/*! \brief Record the stress-strain state of a sample in simulations using TriaxialCompressionEngine

	The output is a text file where each line is a record, with the format 
	IterationNumber sigma11 sigma22 sigma33 epsilon11 epsilon22 epsilon33
	
 */


class PhysicalAction;
class TriaxialCompressionEngine;

class TriaxialStateRecorder : public DataRecorder
{
	private :
		//shared_ptr<PhysicalAction> actionForce; // ??? 
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		std::ofstream ofile;
		
		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;
		
		//Real height, width, depth;
		//Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		
		
		//int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		TriaxialStateRecorder ();

		virtual void registerAttributes();
		virtual void action(Body* b);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(TriaxialStateRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(TriaxialStateRecorder,false);

#endif // WALL_STRESS_RECORDER_HPP

