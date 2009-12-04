// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once 

/*! Engine periodically storing some data to (one) external file.

In addition PeriodicEngine, it handles opening the file as needed.

See PeriodicEngine for controlling periodiciy.
*/
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<fstream>
#include<string>
class Recorder: public PeriodicEngine{
	void openAndCheck();
	protected:
		//! stream object that derived engines should write to
		std::ofstream out;
	public:
		//! Name of file to save to; must not be empty.
		std::string file;
		//! Whether to delete current file contents, if any, when opening (false by default)
		bool truncate;
		Recorder(): truncate(false){}
		virtual bool isActivated(Scene* rb){
			if(PeriodicEngine::isActivated(rb)){
				if(!out.is_open()) openAndCheck();
				return true;}
			return false;
		}
		// virtual void postProcessAttributes(bool);
	REGISTER_CLASS_AND_BASE(Recorder,PeriodicEngine);
	REGISTER_ATTRIBUTES(PeriodicEngine,(file)(truncate));
};
REGISTER_SERIALIZABLE(Recorder);
