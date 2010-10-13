
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
	YADE_CLASS_BASE_DOC_ATTRS(LawTester,PartialEngine,"Prescribe and apply deformations of an interaction in terms of normal and shear displacements. See :ysrc:`scripts/test/law-test.py`. ",
		((vector<Vector3r>,path,,Attr::triggerPostLoad,"Loading path, where each Vector3 contains desired normal displacement and two components of the shear displacement (in local coordinate system, which is being tracked automatically."))
		((vector<string>,hooks,,,"Python commands to be run when the corresponding point in path is reached, before doing other things in that particular step. See also :yref:`doneHook<LawTester.doneHook>`. "))
		((Vector3r,ptOurs,Vector3r::Zero(),,"Current absolute configuration -- computed by ourselves from applied increments; should correspond to posTot."))
		((Vector3r,shearTot,Vector3r::Zero(),,"Current total shear, in global coordinates (computed differently for :yref:`ScGeom` and :yref:`Dem3DofGeom`)."))
		((Vector3r,ptGeom,Vector3r::Zero(),,"Current absolute configuration, in local coordinates; computed from *shearTot*, indirectly  from :yref:`IGeom`. |yupdate|"))
		((bool,displIsRel,true,,"Whether displacement values in *path* are normalized by reference contact length (r1+r2 for 2 spheres)."))
		((Vector3i,forceControl,Vector3i::Zero(),,"Select which components of path (non-zero value) have force (stress) rather than displacement (strain) meaning."))
		((vector<int>,pathSteps,((void)"(constant step)",vector<int>(1,1)),Attr::triggerPostLoad,"Step number for corresponding values in :yref:`path<LawTester.path>`; if shorter than path, distance between last 2 values is used for the rest."))
		((vector<int>,_pathT,,(Attr::readonly|Attr::noSave),"Time value corresponding to points on path, computed from *pathSteps*. Length is always the same as path."))
		((vector<Vector3r>,_pathV,,(Attr::readonly|Attr::noSave),"Path values, computed from *path* by appending zero initial value."))
		((shared_ptr<Interaction>,I,,(Attr::hidden),"Interaction object being tracked."))
		((Vector3r,axX,,,"Local x-axis in global coordinates (normal of the contact) |yupdate|"))
		((Vector3r,axY,,,"Local y-axis in global coordinates; perpendicular to axX; initialized arbitrarily, but tracked to be consistent. |yupdate|"))
		((Vector3r,axZ,,Attr::noSave,"Local z-axis in global coordinates; computed from axX and axY. |yupdate|"))
		((Matrix3r,trsf,,Attr::noSave,"Transformation matrix for the local coordinate system. |yupdate|"))
		((size_t,_interpPos,0,(Attr::readonly),"Position for the interpolation routine."))
		((Vector3r,prevVal,Vector3r::Zero(),(Attr::readonly),"Value reached in the previous step."))
		((Quaternionr,trsfQ,,Attr::noSave,"Transformation quaterion for the local coordinate system. |yupdate|"))
		((int,step,0,,"Step number in which this engine is active; determines position in path, using pathSteps."))
		((string,doneHook,,,"Python command (as string) to run when end of the path is achieved. If empty, the engine will be set :yref:`dead<Engine.dead>`."))
		((Real,renderLength,0,,"Characteristic length for the purposes of rendering, set equal to the smaller radius."))
		((Real,refLength,0,(Attr::readonly),"Reference contact length, for rendering only."))
		((Vector3r,contPt,Vector3r::Zero(),,"Contact point (for rendering only)"))
		((Real,idWeight,1,,"Float ∈〈0,1〉 determining on which particle are displacements applied (0 for id1, 1 for id2); intermediate values will apply respective part to each of them."))
		((Real,rotWeight,1,,"Float ∈〈0,1〉 determining whether shear displacement is applied as rotation or displacement on arc (0 is displacemetn-only, 1 is rotation-only)."))
		// reset force components along individual axes, instead of blocking DOFs which have no specific direction (for the force control)
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

