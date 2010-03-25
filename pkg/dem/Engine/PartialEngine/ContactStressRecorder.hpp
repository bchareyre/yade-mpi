/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/Recorder.hpp>
#include<string>
#include<fstream>

class TriaxialCompressionEngine;

class ContactStressRecorder : public Recorder
{
	private :
		std::ofstream ofile; 
		shared_ptr<TriaxialCompressionEngine> triaxialCompressionEngine;
		
	public :
		virtual void postProcessAttributes(bool deserializing);
		virtual void action();
		virtual bool isActivated();
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ContactStressRecorder,Recorder,"Records informations from contacts on samples submitted to triaxial compressions. -> New formalism needs to be tested!!!",
			((unsigned int,interval,1.,"interval of recording (iterations)"))
			((string,outputFile,"","name of the output file")),
			initRun=true; 
			);
	DECLARE_LOGGER;


};
REGISTER_SERIALIZABLE(ContactStressRecorder);

//// OLD CODE!!!
// class TriaxialCompressionEngine;
// class Shape;
// 
// class ContactStressRecorder : public Recorder
// {
// 	private :
// 		
// 		shared_ptr<Shape> sphere_ptr;
// 		int SpheresClassIndex;
// 		
// 		std::ofstream ofile;
// 		
// 		bool changed;
// 	
// 	public :
// 		std::string	 outputFile;
// 		unsigned int	 interval;
// 		
// 		Real height, width, depth;
// 		Real thickness; // FIXME should retrieve "extents" of a Box
// 		Vector3r upperCorner, lowerCorner;
// 		
// 		shared_ptr<TriaxialCompressionEngine> triaxCompEng;
// 		
// 		int wall_bottom_id, wall_top_id, wall_left_id, wall_right_id, wall_front_id, wall_back_id;
// 
// 		ContactStressRecorder ();
// 
// 		virtual void action();
// 		virtual bool isActivated();
// 
// 	protected :
// 		virtual void postProcessAttributes(bool deserializing);
// 	REGISTER_ATTRIBUTES(Recorder,( outputFile )( interval )( wall_bottom_id )( wall_top_id )( wall_left_id )( wall_right_id )( wall_front_id )( wall_back_id )( height )( width )( depth )( thickness )( upperCorner )( lowerCorner ));
// 	DECLARE_LOGGER;
// 	REGISTER_CLASS_NAME(ContactStressRecorder);
// 	REGISTER_BASE_CLASS_NAME(Recorder);
// };
// 
// REGISTER_SERIALIZABLE(ContactStressRecorder);


