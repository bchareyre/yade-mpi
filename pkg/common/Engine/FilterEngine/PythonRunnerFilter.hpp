/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/FilterEngine.hpp>
#include<Python.h>

// Run python command
class PythonRunnerFilter: public FilterEngine {
	private:
		string command;
	public :
		PythonRunnerFilter(): command("pass"){};
		virtual void action(MetaBody* b){
			PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyRun_SimpleString(command.c_str()); 
			PyGILState_Release(gstate);
		}
		virtual void registerAttributes(){ FilterEngine::registerAttributes(); REGISTER_ATTRIBUTE(command); }
	protected :
		virtual void postProcessAttributes(bool deserializing){}
	REGISTER_CLASS_NAME(PythonRunnerFilter);
	REGISTER_BASE_CLASS_NAME(FilterEngine);
};

REGISTER_SERIALIZABLE(PythonRunnerFilter);

