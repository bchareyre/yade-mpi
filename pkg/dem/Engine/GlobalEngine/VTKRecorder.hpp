#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>


class VTKRecorder: public PeriodicEngine {
	public:
		enum {REC_SPHERES=0,REC_FACETS,REC_COLORS,REC_CPM,REC_INTR,REC_VELOCITY,REC_IDS,REC_CLUMPIDS,REC_SENTINEL};
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(VTKRecorder,PeriodicEngine,"Engine recording snapshots of simulation into series of *.vtu files, readable by VTK-based postprocessing programs such as Paraview. Both bodies (spheres and facets) and interactions can be recorded, with various vector/scalar quantities that are defined on them.\n\n:yref:`PeriodicEngine.initRun` is initialized to ``True`` automatically.",
		((bool,compress,false,"Compress output XML files [experimental]."))
		((bool,skipFacetIntr,true,"Skip interactions with facets, when saving interactions"))
		((bool,skipNondynamic,false,"Skip non-dynamic spheres (but not facets)."))
		((string,fileName,"","Base file name; it will be appended with {spheres,intrs,facets}-243100.vtu depending on active recorders and step number (243100 in this case). It can contain slashes, but the directory must exist already."))
		((vector<string>,recorders,,"List of active recorders (as strings). Accepted recorders are:\n\n``spheres``\n\tSaves positions and radii (``radii``) of :yref:`spherical<Sphere>` particles.\n``ids``\n\tSaves id's (field ``IDS``) of spheres; active only if ``spheres`` is active.\n``clumpids``\n\tSaves id's of clumps to which each sphere belongs (field ``clumpIDS``); active only if ``spheres`` is active.\n``colors``\n\tSaves colors of :yref:`spheres<Sphere>` and of :yref:`facets<Facet>` (field ``colors`` spheres and ``Colors`` for facets); only active if ``spheres`` or ``facets`` is activated.\n``velocity``\n\tSaves linear and angular velocities of spherical particles (fields ``velocity`` and ``angVel`` respectively``); only effective with ``spheres``.\n``facets``\n\tSave :yref:`facets<Facet>` positions (vertices).\n``cpm``\n\tSaves data pertaining to the :yref:`concrete model<Law2_Dem3DofGeom_CpmPhys_Cpm>`: ``cpmDamage`` (normalized residual strength averaged on particle), ``cpmSigma`` (stress on particle, normal components), ``cpmTau`` (shear components of stress on particle), ``cpmSigmaM`` (mean stress around particle); ``intr`` is activated automatically by ``cpm``.\n``intr``\n\tWhen ``cpm`` is used, it saves magnitude of normal (``forceN``) and shear (``absForceT``) forces.\n\n\tWithout ``cpm``, saves [TODO]\n\n")),
		/*ctor*/ initRun=true;
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(VTKRecorder);

