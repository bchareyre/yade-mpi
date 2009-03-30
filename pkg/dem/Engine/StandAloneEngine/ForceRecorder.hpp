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


class ForceRecorder : public DataRecorder
{
	private :
		std::ofstream ofile; 

		bool changed;
	
		bool first;
	public :
		std::string	 outputFile;
		int		 interval
				,startId
				,endId;

		ForceRecorder ();

		virtual void registerAttributes();
		virtual void action(MetaBody*);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void init();
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(ForceRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(ForceRecorder);


