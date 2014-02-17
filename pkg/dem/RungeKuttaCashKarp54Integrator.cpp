#include<yade/pkg/dem/RungeKuttaCashKarp54Integrator.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((RungeKuttaCashKarp54Integrator));

shared_ptr<Integrator> RungeKuttaCashKarp54Integrator_ctor_list(const python::list& slaves){ shared_ptr<Integrator> instance(new RungeKuttaCashKarp54Integrator); instance->slaves_set(slaves); return instance; }

void RungeKuttaCashKarp54Integrator::action()
{

	Real dt=scene->dt;

	Real time=scene->time;

	error_checker_type rungekuttaerrorcontroller=error_checker_type(abs_err,rel_err,a_x,a_dxdt);
	
	controlled_stepper_type rungekuttastepper=controlled_stepper_type(rungekuttaerrorcontroller);

	stateVector currentstates=getCurrentStates();
	
	resetstate.reserve(currentstates.size());
	
	copy(currentstates.begin(),currentstates.end(),back_inserter(resetstate));//copy current state to resetstate

	this->timeresetvalue=time; //set reset time to the time just before the integration
	
	/*Try an adaptive integration*/

	integrationsteps+=integrate_adaptive(rungekuttastepper,make_ode_wrapper( *((Integrator*)this) , &Integrator::system ),currentstates,time,time+dt, stepsize, observer(this));

	scene->time=scene->time-dt;//Scene move next time step function already increments the time so we have to decrement it just before it.
}

