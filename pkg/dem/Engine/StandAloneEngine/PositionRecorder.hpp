/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DataRecorder.hpp>
#include <string>
#include <fstream>

class PositionRecorder : public DataRecorder
{
	private :
		std::ofstream ofile; 
	public :
		std::string outputFile;
		unsigned int	 interval
				,startId
				,endId;
	
		PositionRecorder ();
		~PositionRecorder ();
		virtual void action(MetaBody*);
		virtual bool isActivated(MetaBody*);

	protected :
		virtual void postProcessAttributes(bool deserializing);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(PositionRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(PositionRecorder);


