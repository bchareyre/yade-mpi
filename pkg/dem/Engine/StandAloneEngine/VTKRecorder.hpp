#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>


class VTKRecorder: public PeriodicEngine {
	public:
		enum {REC_SPHERES=0,REC_FACETS,REC_COLORS,REC_CPM,REC_INTR,REC_VELOCITY,REC_IDS,REC_SENTINEL};
		//! A stuff to record: spheres,facets,colors 
		vector<string> recorders;
		string fileName;
		//! turn on compression of the output XML files
		bool compress;
		//! skip interactions with facets
		bool skipFacetIntr;
		//! skip non-dynamic spheres (not facets)
		bool skipNondynamic;
		VTKRecorder(); 
		~VTKRecorder();
		void init(Scene*);
		virtual void action(Scene*);
	private:
		
	REGISTER_ATTRIBUTES(PeriodicEngine,(recorders)(fileName)(compress)(skipNondynamic)(skipFacetIntr));
	REGISTER_CLASS_AND_BASE(VTKRecorder,PeriodicEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VTKRecorder);

