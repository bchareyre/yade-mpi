#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>


class VTKRecorder: public PeriodicEngine {
	public:
		enum {REC_SPHERES=0,REC_FACETS,REC_COLORS,REC_CPM,REC_INTR,REC_VELOCITY,REC_IDS,REC_CLUMPIDS,REC_SENTINEL};
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(VTKRecorder,PeriodicEngine,"Engine recording snapshots of simulation into series of *.vtu files, readable by VTK-based postprocessing programs such as Paraview. Both bodies (spheres and facets) and interactions can be recorded, with various vector/scalar quantities that are defined on them.",
		((bool,compress,false,"Compress output XML files [experimental]."))
		((bool,skipFacetIntr,true,"Skip interactions with facets, when saving interactions"))
		((bool,skipNondynamic,false,"Skip non-dynamic spheres (but not facets)."))
		((string,fileName,"","Base file name; it will be appended with {spheres,intrs,facets}-243100.vtu depending on active recorders and step number (243100 in this case). It can contain slashes, but the directory must exist already."))
		((vector<string>,recorders,,"List of active recorders (as strings). Acceptable recorders are spheres, velocity, facets, colors, cpm, intr, ids, clumpids.")),
		/*ctor*/ initRun=true;
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VTKRecorder);

