// © 2007 Václav Šmilauer <eudoxos@arcig.cz>
#ifndef CMDGUI_HPP
#define CMDGUI_HPP
#include<yade/core/FrontEnd.hpp>
#include<boost/shared_ptr.hpp>

class cmdGui: public FrontEnd
{
	private:
		void help();
		void execScript(string);
	public:
		cmdGui(){};
		virtual ~cmdGui(){};
		virtual int run(int argc, char *argv[]);
	
	REGISTER_CLASS_NAME(cmdGui);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
	DECLARE_LOGGER;
};

REGISTER_FACTORABLE(cmdGui);

#endif
