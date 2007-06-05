/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CAPILLARY_STRESS_RECORDER_HPP
#define CAPILLARY_STRESS_RECORDER_HPP

#include <yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

class GeometricalModel;

class CapillaryStressRecorder : public DataRecorder
{
	private :
		std::ofstream ofile; 

		bool changed;
		shared_ptr<GeometricalModel> sphere_ptr;
		int SpheresClassIndex;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;
		
		Real height, width, depth;
		Real thickness; // FIXME should retrieve "extents" of a InteractingBox
		
		Vector3r upperCorner, lowerCorner;

		int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;

		CapillaryStressRecorder ();

		virtual void registerAttributes();
		virtual void action(Body* b);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(CapillaryStressRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(CapillaryStressRecorder,false);

#endif // CAPILLARY_STRESS_RECORDER_HPP

