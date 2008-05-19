// © 2007 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/FrontEnd.hpp>
#include<boost/shared_ptr.hpp>
#include<termios.h>

class PythonUI: public FrontEnd
{
	private:
		void help();
		static void execScript(string);
		static struct termios tios,tios_orig;
		static void termSetup();
		static void termRestore();
		static string runScript, runCommands;
		static bool stopAfter;
	public:
		PythonUI(){};
		virtual ~PythonUI(){};
		static void pythonSession();
		virtual int run(int argc, char *argv[]);
	
	REGISTER_CLASS_NAME(PythonUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
	DECLARE_LOGGER;
};

REGISTER_FACTORABLE(PythonUI);

