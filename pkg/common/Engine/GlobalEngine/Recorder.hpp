// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once 
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<fstream>
#include<string>
class Recorder: public PeriodicEngine{
	void openAndCheck();
	protected:
		//! stream object that derived engines should write to
		std::ofstream out;
	public:
		virtual ~Recorder(); // vtable
		virtual bool isActivated(){
			if(PeriodicEngine::isActivated()){
				if(!out.is_open()) openAndCheck();
				return true;
			}
			return false;
		}
	YADE_CLASS_BASE_DOC_ATTRS(Recorder,PeriodicEngine,"Engine periodically storing some data to (one) external file. In addition PeriodicEngine, it handles opening the file as needed. See :yref:`PeriodicEngine` for controlling periodicity.",
		((std::string,file,,"Name of file to save to; must not be empty."))
		((bool,truncate,false,"Whether to delete current file contents, if any, when opening (false by default)"))
		((bool,addIterNum,false,"Adds an iteration number to the file name, when the file was created. Useful for creating new files at each call (false by default)"))
	);
};
REGISTER_SERIALIZABLE(Recorder);
