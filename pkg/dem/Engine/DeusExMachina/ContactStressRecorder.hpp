/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

class GeometricalModel;
class TriaxialCompressionEngine;
// class SampleCapillaryPressureEngine;


class ContactStressRecorder : public DataRecorder
{
	private :
		
		shared_ptr<GeometricalModel> sphere_ptr;
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

		virtual void registerAttributes();
		virtual void action(MetaBody*);
		virtual bool isActivated(MetaBody*);

	protected :
		virtual void postProcessAttributes(bool deserializing);
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(ContactStressRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(ContactStressRecorder);


