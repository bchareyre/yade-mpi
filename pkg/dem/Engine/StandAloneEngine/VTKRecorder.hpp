#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>


class VTKRecorder: public PeriodicEngine {
	public:
		enum {REC_SPHERES=0,REC_FACETS,REC_COLORS,REC_SENTINEL};
		//! A stuff to record: spheres,facets,colors 
		vector<string> recorders;
		string fileName;
		VTKRecorder(); 
		~VTKRecorder();
		void init(MetaBody*);
		virtual void action(MetaBody*);
	private:
		
	REGISTER_ATTRIBUTES(PeriodicEngine,(recorders)(fileName));
	REGISTER_CLASS_AND_BASE(VTKRecorder,PeriodicEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VTKRecorder);

