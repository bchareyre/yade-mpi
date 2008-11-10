// © 2007 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/FrontEnd.hpp>
#include<termios.h>

class PythonUI: public FrontEnd
{
	private:
		void help();
		static void execScript(string);
		static struct termios tios,tios_orig;
		static void termSetup();
		static void termRestore();
	public:
		PythonUI(){self=this; nonInteractive=false; stopAfter=false; };
		virtual ~PythonUI(){};
		static void pythonSession();
		static PythonUI *self;
		virtual int run(int argc, char *argv[]);
		static string runScript;
		// stopAfter and nonInteractive are almost identical, should be clarified/fixed
		static bool stopAfter;
		static bool nonInteractive;
		static vector<string> scriptArgs;
	
	REGISTER_CLASS_NAME(PythonUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
	DECLARE_LOGGER;
};

REGISTER_FACTORABLE(PythonUI);

