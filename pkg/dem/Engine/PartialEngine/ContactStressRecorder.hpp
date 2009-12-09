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
// class SampleCapillaryPressureEngine;


class ContactStressRecorder : public Recorder
{
	private :
		
		shared_ptr<Shape> sphere_ptr;
		int SpheresClassIndex;
		
		std::ofstream ofile;
		
		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;
		
		Real height, width, depth;
		Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		Vector3r upperCorner, lowerCorner;
		
		shared_ptr<TriaxialCompressionEngine> triaxCompEng;
		//SampleCapillaryPressureEngine* sampleCapPressEng; 
		
		int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		ContactStressRecorder ();

		virtual void action(Scene*);
		virtual bool isActivated(Scene*);

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Recorder,( outputFile )( interval )( wall_bottom_id )( wall_top_id )( wall_left_id )( wall_right_id )( wall_front_id )( wall_back_id )( height )( width )( depth )( thickness )( upperCorner )( lowerCorner ));
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(ContactStressRecorder);
	REGISTER_BASE_CLASS_NAME(Recorder);
};

REGISTER_SERIALIZABLE(ContactStressRecorder);


