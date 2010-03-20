/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg-common/Recorder.hpp>

#include <string>
#include <fstream>

class TriaxialCompressionEngine;
class Shape;

class CapillaryStressRecorder : public Recorder
{
	private :
		std::ofstream ofile; 
		
		bool changed;
		shared_ptr<Shape> sphere_ptr;
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		int SpheresClassIndex;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;
		
		Real height, width, depth;
		Real thickness; // FIXME should retrieve "extents" of a Box
		
		//Vector3r upperCorner, lowerCorner;

		int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		CapillaryStressRecorder ();

		virtual void action();
		virtual bool isActivated();
	DECLARE_LOGGER;
	REGISTER_ATTRIBUTES(Recorder,(outputFile)(interval)/*(wall_bottom_id)(wall_top_id)(wall_left_id)(wall_right_id)(wall_front_id)(wall_back_id)(height)(width)(depth)(thickness)(upperCorner)(lowerCorner)	*/);
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(CapillaryStressRecorder);
	REGISTER_BASE_CLASS_NAME(Recorder);
};

REGISTER_SERIALIZABLE(CapillaryStressRecorder);


