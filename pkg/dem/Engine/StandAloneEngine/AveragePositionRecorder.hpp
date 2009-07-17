/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

class AveragePositionRecorder : public DataRecorder
{
	public :
		AveragePositionRecorder ();
		
		virtual void action(MetaBody*);
		virtual bool isActivated(MetaBody*);
	
	protected :
		virtual void postProcessAttributes(bool deserializing);


	public : 
		std::string outputFile;
		unsigned int interval;
	private : 
		std::ofstream ofile;
	REGISTER_ATTRIBUTES(DataRecorder,(outputFile)(interval));
	REGISTER_CLASS_NAME(AveragePositionRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);

};

REGISTER_SERIALIZABLE(AveragePositionRecorder);


