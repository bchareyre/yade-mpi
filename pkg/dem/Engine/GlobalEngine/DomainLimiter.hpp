
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/core/PartialEngine.hpp>

class DomainLimiter: public PeriodicEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(DomainLimiter,PeriodicEngine,"Delete particles that are out of axis-aligned box given by *lo* and *hi*.",
		((Vector3r,lo,Vector3r(0,0,0),,"Lower corner of the domain."))
		((Vector3r,hi,Vector3r(0,0,0),,"Upper corner of the domain."))
		((long,nDeleted,0,Attr::readonly,"Cummulative number of particles deleted."))
	);
};
REGISTER_SERIALIZABLE(DomainLimiter);

class LawTester: public PartialEngine{
	Body::id_t id1,id2; // shorthands for local use
	public:
		void init();
		virtual void action();
		void postLoad(LawTester&);
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(LawTester,PartialEngine,"Prescribe and apply deformations of an interaction in terms of normal and shear displacements.",
		((vector<Vector3r>,path,,Attr::triggerPostLoad,"Loading path, where each Vector3 contains desired normal displacement and two components of the shear displacement (in local coordinate system, which is being tracked automatically."))
		((Vector3r,ptOurs,Vector3r::Zero(),,"Current absolute configuration -- computed by ourselves from applied increments; should correspond to posTot."))
		((Vector3r,shearTot,Vector3r::Zero(),,"Current total shear, in global coordinates (computed differently for :yref:`ScGeom` and :yref:`Dem3DofGeom`)."))
		((Vector3r,ptGeom,Vector3r::Zero(),,"Current absolute configuration, in local coordinates; computed from *shearTot*, indirectly  from :yref:`IGeom`. |yupdate|"))
		((bool,pathIsRel,true,,"Whether values in *path* are normalized by contact length."))
		((vector<int>,pathSteps,((void)"(constant step)",vector<int>(1,1)),Attr::triggerPostLoad,"Step number for corresponding values in :yref:`path<LawTester.path>`; if shorter than path, distance between last 2 values is used for the rest."))
		((vector<int>,_pathT,,(Attr::readonly|Attr::noSave),"Time value corresponding to points on path, computed from *pathSteps*. Length is always the same as path."))
		((vector<Vector3r>,_pathV,,(Attr::readonly|Attr::noSave),"Path values, computed from *path* by appending zero initial value."))
		((shared_ptr<Interaction>,I,,,"Interaction object being tracked."))
		((Vector3r,axX,,,"Local x-axis in global coordinates (normal of the contact) |yupdate|"))
		((Vector3r,axY,,,"Local y-axis in global coordinates; perpendicular to axX; initialized arbitrarily, but tracked to be consistent. |yupdate|"))
		((Vector3r,axZ,,Attr::noSave,"Local z-axis in global coordinates; computed from axX and axY. |yupdate|"))
		((Matrix3r,trsf,,Attr::noSave,"Transformation matrix for the local coordinate system. |yupdate|"))
		((size_t,_interpPos,0,(Attr::readonly),"Position for the interpolation routine."))
		((Vector3r,prevVal,Vector3r::Zero(),(Attr::readonly),"Value reached in the previous step."))
		((Quaternionr,trsfQ,,Attr::noSave,"Transformation quaterion for the local coordinate system. |yupdate|"))
		((int,step,0,,"Step number in which this engine is active; determines position in path, either using pathStep or pathSteps."))
		((string,doneHook,,,"Python command (as string) to run when end of the path is achieved. If empty, the engine will be set :yref:`dead<Engine.dead>`."))
		((Real,renderLength,0,,"Characteristic length for the purposes of rendering, set equal to the smaller radius."))
		((Vector3r,contPt,Vector3r::Zero(),,"Contact point (for rendering only)"))
		//((int,sense,1,,"Determines what particle is moved: negative for id1, positive for id2, 0 for both."))
	);
};
REGISTER_SERIALIZABLE(LawTester);

#ifdef YADE_OPENGL
#include<yade/pkg-common/OpenGLRenderer.hpp>

class GlExtra_LawTester: public GlExtraDrawer{
	public:
	DECLARE_LOGGER;
	virtual void render();
	YADE_CLASS_BASE_DOC_ATTRS(GlExtra_LawTester,GlExtraDrawer,"Find an instance of :yref:`LawTester` and show visually its data.",
		((shared_ptr<LawTester>,tester,,,"Associated :yref:`LawTester` object."))
	);
};
REGISTER_SERIALIZABLE(GlExtra_LawTester);
#endif

