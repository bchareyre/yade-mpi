#pragma once
#include <yade/core/Scene.hpp>
#include<yade/pkg/dem/Integrator.hpp>
#include<boost/numeric/odeint.hpp>


typedef boost::numeric::odeint::runge_kutta_cash_karp54< stateVector > error_stepper_type; //Runge-Kutta 54 error stepper other steppers can also be used

typedef boost::numeric::odeint::controlled_runge_kutta< error_stepper_type > controlled_stepper_type;//Controlled Runge Kutta stepper

typedef boost::numeric::odeint::default_error_checker< error_stepper_type::value_type,error_stepper_type::algebra_type ,error_stepper_type::operations_type > error_checker_type; //Error checker type that is redefined for initialization using different tolerance values


shared_ptr<Integrator> RungeKuttaCashKarp54Integrator_ctor_list(const python::list& slaves);
class RungeKuttaCashKarp54Integrator: public Integrator {
	
	public:
	
		error_checker_type rungekuttaerrorcontroller;
	
		controlled_stepper_type rungekuttastepper;

		void init()
		{
			rungekuttaerrorcontroller=error_checker_type(abs_err,rel_err,a_x,a_dxdt);
			rungekuttastepper=controlled_stepper_type(rungekuttaerrorcontroller);
		}



		virtual void action();

		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(RungeKuttaCashKarp54Integrator,Integrator,"RungeKuttaCashKarp54Integrator engine.",
		((Real,abs_err,1e-6,,"Relative integration tolerance"))
		((Real,rel_err,1e-6,,"Absolute integration tolerance"))		
		((Real,a_x,1.0,,""))
		((Real,a_dxdt,1.0,,""))
		((Real,stepsize,1e-6,,"It is not important for an adaptive integration but important for the observer for setting the found states after integration"))
		,
		/*ctor*/
		init();
		,
		.def("__init__",python::make_constructor(RungeKuttaCashKarp54Integrator_ctor_list),"Construct from (possibly nested) list of slaves.")
		/*py*/
	);
};
REGISTER_SERIALIZABLE(RungeKuttaCashKarp54Integrator);



