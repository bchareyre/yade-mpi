// Kovthanan …
#include<yade/lib/base/Math.hpp>
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg/common/Collider.hpp>


class NozzleFactory: public GlobalEngine {
	shared_ptr<Collider> collider;
	public:
		virtual void action();
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(NozzleFactory,GlobalEngine,"Engine for spitting spheres based on mass flow rate, particle size distribution etc. The area where spehres are generated should be circular, given by radius, center and normal. For now, axis-aligned cube-shape corresponding is used instead, centered at *center* and with size $\\frac{2\\sqrt{3}}{3}$. Initial velocity of particles is given by *vMin*, *vMax*, the *massFlowRate* determines how many particles to generate at each step. When *goalMass* is attained or positive *maxParticles* is reached, the engine does not produce particles anymore.\n\nA sample script for this engine is in :ysrc:`scripts/shots.py`.",
		((Real,massFlowRate,NaN,,"Mass flow rate [kg/s]"))
		((Real,rMin,NaN,,"Minimum radius of generated spheres (uniform distribution)"))
		((Real,rMax,NaN,,"Maximum radius of generated spheres (uniform distribution)"))
		((Real,vMin,NaN,,"Minimum velocity norm of generated spheres (uniform distribution)"))
		((Real,vMax,NaN,,"Maximum velocity norm of generated spheres (uniform distribution)"))
		((Real,vAngle,NaN,,"Maximum angle by which the initial sphere velocity deviates from the nozzle normal."))
		((Real,radius,NaN,,"Radius of the nozzle"))
		((int,materialId,-1,,"Shared material id to use for newly created spheres (can be negative to count from the end)"))
		((Vector3r,center,Vector3r(NaN,NaN,NaN),,"Center of the nozzle"))
		((Vector3r,normal,Vector3r(NaN,NaN,NaN),,"Spitting direction, i.e. normal of the circle where spheres are generated."))
		((Real,totalMass,0,,"Mass of spheres that was produced so far. |yupdate|"))
		((Real,goalMass,0,,"Total mass that should be attained at the end of the current step. |yupdate|"))
		((int,maxParticles,100,,"The number of particles at which to stop generating new ones (regardless of massFlowRate"))
		((int,numParticles,0,,"Cummulative number of particles produces so far |yupdate|"))
		((int,maxAttempt,5000 ,,"Maximum number of attempts to position a new sphere randomly."))
	);
};
REGISTER_SERIALIZABLE(NozzleFactory);

class DragForceApplier: public GlobalEngine{
	public: virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(DragForceApplier,GlobalEngine,"Apply `drag force <http://en.wikipedia.org/wiki/Drag_equation>`__ on particles, decelerating them proportionally to their linear velocities. The applied force reads\n\n.. math:: F_{d}=-\\frac{\\vec{v}}{|\\vec{v}|}\\frac{1}{2}\\rho|\\vec{v}|^2 C_d A\n\nwhere $\\rho$ is the medium density (:yref:`density<DragForceApplier.density>`), $v$ is particle's velocity,  $A$ is particle projected area (disc), $C_d$ is the drag coefficient (0.47 for :yref:`Sphere`), \n\n.. note:: Drag force is only applied to spherical particles.\n\n.. warn:: Not tested.",
		((Real,density,0,,"Density of the medium."))
	);
};
REGISTER_SERIALIZABLE(DragForceApplier);
