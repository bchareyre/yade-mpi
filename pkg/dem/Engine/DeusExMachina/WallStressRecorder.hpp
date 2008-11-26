/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef WALL_STRESS_RECORDER_HPP
#define WALL_STRESS_RECORDER_HPP

#include <yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

class PhysicalAction;

class WallStressRecorder : public DataRecorder
{
	private :
		shared_ptr<PhysicalAction> actionForce; // ??? 
		std::ofstream ofile;
		
		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;
		
		Real height, width, depth;
		Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		
		
		int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		WallStressRecorder ();

		virtual void registerAttributes();
		virtual void action(MetaBody*);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	NEEDS_BEX("Force");
	REGISTER_CLASS_NAME(WallStressRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(WallStressRecorder);

#endif // WALL_STRESS_RECORDER_HPP

