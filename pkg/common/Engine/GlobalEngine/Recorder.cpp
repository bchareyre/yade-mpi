// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/pkg-common/Recorder.hpp>
YADE_PLUGIN((Recorder));

Recorder::~Recorder(){}
void Recorder::openAndCheck(){
	assert(!out.is_open());
	if(file.empty()) throw ios_base::failure(__FILE__ ": Empty filename.");
	out.open(file.c_str(), truncate ? fstream::trunc : fstream::app);
	if(!out.good()) throw ios_base::failure(__FILE__ ": I/O error opening file `"+file+"'.");
}
#if 0
	void Recorder::postProcessAttributes(bool deserializing){
		if(deserializing) openAndCheck();
		else{ if(out.is_open()) out.flush(); }
	}
#endif
