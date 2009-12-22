#include <string.h>
#include "Timer.h"

Real_timer::Real_timer() : T1(0), T2(0), elapsed(0.0), started(0.0), interv(0), running(true)
{
	T1 = clock();
}


// Member functions for Real_timer
// ===========================

void Real_timer::start() {
    //CGAL_precondition( ! running);
    //started = get_real_time();
    T1=clock();
	running = true;
    ++ interv;
}

void Real_timer::stop() {
    //CGAL_precondition( running);
    //double t = get_real_time();
    //elapsed += (t - started);
	T2 = difftime(clock(), T1);
    started = 0.0;
    T1=0;
	running = false;
}

void Real_timer::reset() {
    interv  = 0;
    elapsed = 0.0;
	T2=0;
    if (running) {
	//started = get_real_time();
	T1=clock();
	++ interv;
    } else {
        started = 0.0;
		T1=0;
    }
}

double Real_timer::time() const {
    if (running) {
		clock_t  T3 = clock();
		return difftime(T3, T1);
    }
    return T2;
}

void	Real_timer::top(string Texte)
{
	clock_t  T3 = clock();
	double r = (double) (T3 - T1)/ CLOCKS_PER_SEC;
	std::cout << Texte << " .......... " << r << " s" << std::endl;
	reset();
}
