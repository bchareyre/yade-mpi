#pragma once
#include<yade/pkg-common/PeriodicEngines.hpp>


class VTKRecorder: public PeriodicEngine {
	public:
		enum {REC_SPHERES=0,REC_FACETS,REC_COLORS,REC_CPM,REC_INTR,REC_VELOCITY,REC_ID,REC_CLUMPID,REC_SENTINEL,REC_MATERIALID,REC_FORCE};
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(VTKRecorder,PeriodicEngine,"Engine recording snapshots of simulation into series of *.vtu files, readable by VTK-based postprocessing programs such as Paraview. Both bodies (spheres and facets) and interactions can be recorded, with various vector/scalar quantities that are defined on them.\n\n:yref:`PeriodicEngine.initRun` is initialized to ``True`` automatically.",
		((bool,compress,false,"Compress output XML files [experimental]."))
		((bool,skipFacetIntr,true,"Skip interactions with facets, when saving interactions"))
		((bool,skipNondynamic,false,"Skip non-dynamic spheres (but not facets)."))
		((string,fileName,"","Base file name; it will be appended with {spheres,intrs,facets}-243100.vtu depending on active recorders and step number (243100 in this case). It can contain slashes, but the directory must exist already."))
		((vector<string>,recorders,,"List of active recorders (as strings). Accepted recorders are:\n\n``all``\n\tSaves all possible parameters, except of specific. Default value.\n``spheres``\n\tSaves positions and radii (``radii``) of :yref:`spherical<Sphere>` particles.\n``id``\n\tSaves id's (field ``id``) of spheres; active only if ``spheres`` is active.\n``clumpId``\n\tSaves id's of clumps to which each sphere belongs (field ``clumpId``); active only if ``spheres`` is active.\n``colors``\n\tSaves colors of :yref:`spheres<Sphere>` and of :yref:`facets<Facet>` (field ``color``); only active if ``spheres`` or ``facets`` is activated.\n``materialId``\n\tSaves materialID of :yref:`spheres<Sphere>` and of :yref:`facets<Facet>`; only active if ``spheres`` or ``facets`` is activated.\n``velocity``\n\tSaves linear and angular velocities of spherical particles as Vector3 and length(fields ``linVelVec``, ``linVelLen`` and ``angVelVec``, ``angVelLen`` respectively``); only effective with ``spheres``.\n``facets``\n\tSave :yref:`facets<Facet>` positions (vertices).\n``force``\n\tSaves forces of :yref:`spheres<Sphere>` and of :yref:`facets<Facet>`  as Vector3 and length; only active if ``spheres`` or ``facets`` is activated.\n``cpm``\n\tSaves data pertaining to the :yref:`concrete model<Law2_Dem3DofGeom_CpmPhys_Cpm>`: ``cpmDamage`` (normalized residual strength averaged on particle), ``cpmSigma`` (stress on particle, normal components), ``cpmTau`` (shear components of stress on particle), ``cpmSigmaM`` (mean stress around particle); ``intr`` is activated automatically by ``cpm``.\n``intr``\n\tWhen ``cpm`` is used, it saves magnitude of normal (``forceN``) and shear (``absForceT``) forces.\n\n\tWithout ``cpm``, saves [TODO]\n\n"))
		((int,mask,0,"If mask defined, only bodies with corresponding groupMask will be exported. If 0 - all bodies will be exported.")),
		/*ctor*/
		initRun=true;
		if (recorders.empty()) {recorders.push_back("all");}	//Default value
	);
	DECLARE_LOGGER;
};

//Class for storing forces, affected on bodies, obtained from Interactions
class bodyForce{
	public:
		Vector3r norm, shear;
		bodyForce (){
			norm = Vector3r(0.0,0.0,0.0);
			shear = Vector3r(0.0,0.0,0.0);
		}
};
REGISTER_SERIALIZABLE(VTKRecorder);
