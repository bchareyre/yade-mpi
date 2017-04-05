// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 


#pragma once

#include<core/PartialEngine.hpp>

class ForceEngine: public PartialEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(ForceEngine,PartialEngine,"Apply contact force on some particles at each step.",
		((Vector3r,force,Vector3r::Zero(),,"Force to apply."))
	);
};
REGISTER_SERIALIZABLE(ForceEngine);

/* Engine for applying force of varying magnitude but constant direction
 * on subscribed bodies. times and magnitudes must have the same length,
 * direction (normalized automatically) gives the orientation.
 *
 * As usual with interpolating engines: the first magnitude is used before the first
 * time point, last magnitude is used after the last time point. Wrap specifies whether
 * time wraps around the last time point to the first time point.
 */
class InterpolatingDirectedForceEngine: public ForceEngine{
	size_t _pos;
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InterpolatingDirectedForceEngine,ForceEngine,"Engine for applying force of varying magnitude but constant direction on subscribed bodies. times and magnitudes must have the same length, direction (normalized automatically) gives the orientation. \n\n\
	\
	As usual with interpolating engines: the first magnitude is used before the first time point, last magnitude is used after the last time point. Wrap specifies whether time wraps around the last time point to the first time point.",
		((vector<Real>,times,,,"Time readings [s]"))
		((vector<Real>,magnitudes,,,"Force magnitudes readings [N]"))
		((Vector3r,direction,Vector3r::UnitX(),,"Contact force direction (normalized automatically)"))
		((bool,wrap,false,,"wrap to the beginning of the sequence if beyond the last time point")),
		/*ctor*/ _pos=0
	);
};
REGISTER_SERIALIZABLE(InterpolatingDirectedForceEngine);

struct RadialForceEngine: public PartialEngine{
	virtual void action();
	virtual void postLoad(RadialForceEngine&);
	YADE_CLASS_BASE_DOC_ATTRS(RadialForceEngine,PartialEngine,"Apply force of given magnitude directed away from spatial axis.",
		((Vector3r,axisPt,Vector3r::Zero(),,"Point on axis"))
		((Vector3r,axisDir,Vector3r::UnitX(),Attr::triggerPostLoad,"Axis direction (normalized automatically)"))
		((Real,fNorm,0,,"Applied force magnitude"))
	);
};
REGISTER_SERIALIZABLE(RadialForceEngine);

class DragEngine: public PartialEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(DragEngine,PartialEngine,"Apply `drag force <http://en.wikipedia.org/wiki/Drag_equation>`__ on some particles at each step, decelerating them proportionally to their linear velocities. The applied force reads\n\n.. math:: F_{d}=-\\frac{\\vec{v}}{|\\vec{v}|}\\frac{1}{2}\\rho|\\vec{v}|^2 C_d A\n\nwhere $\\rho$ is the medium density (:yref:`density<DragEngine.Rho>`), $v$ is particle's velocity,  $A$ is particle projected area (disc), $C_d$ is the drag coefficient (0.47 for :yref:`Sphere`), \n\n.. note:: Drag force is only applied to spherical particles, listed in ids.",
		((Real,Rho,1.225,,"Density of the medium (fluid or air), by default - the density of the air."))
		((Real,Cd,0.47,,"Drag coefficient <http://en.wikipedia.org/wiki/Drag_coefficient>`_."))
	);
};
REGISTER_SERIALIZABLE(DragEngine);

class LinearDragEngine: public PartialEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(LinearDragEngine,PartialEngine,"Apply `viscous resistance or linear drag <http://en.wikipedia.org/wiki/Drag_%28physics%29#Very_low_Reynolds_numbers_.E2.80.94_Stokes.27_drag>`__ on some particles at each step, decelerating them proportionally to their linear velocities. The applied force reads\n\n.. math:: F_{d}=-b{\\vec{v}} \n\nwhere $b$ is the linear drag, $\\vec{v}$ is particle's velocity. \n\n.. math:: b=6\\pi\\nu r \n\nwhere $\\nu$ is the medium viscosity, $r$ is the `Stokes radius <http://en.wikipedia.org/wiki/Stokes_radius>`__ of the particle (but in this case we accept it equal to sphere radius for simplification), \n\n.. note:: linear drag is only applied to spherical particles, listed in ids.",
		((Real,nu,0.001,,"Viscosity of the medium."))
	);
};
REGISTER_SERIALIZABLE(LinearDragEngine);
