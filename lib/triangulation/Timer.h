#ifndef bru_REAL_TIMER_H
#define bru_REAL_TIMER_H 1

//#include <CGAL/basic.h>
//#include <float.h>
//#include "stdafx.h"
#include <iostream>
//#include <tchar.h>
#include <time.h>

//using namespace std;

//// crt_clock.c
///* This example prompts for how long
// * the program is to run and then continuously
// * displays the elapsed time for that period.
// */
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//
//void sleep( clock_t wait );
//
//int main( void )
//{
//   long    i = 6000000L;
//   clock_t start, finish;
//   double  duration;
//
//   /* Delay for a specified time. */
//   printf( "Delay for three seconds\n" );
//   sleep( (clock_t)3 * CLOCKS_PER_SEC );
//   printf( "Done!\n" );
//
//   /* Measure the duration of an event. */
//   printf( "Time to do %ld empty loops is ", i );
//   start = clock();
//   while( i-- ) 
//      ;
//   finish = clock();
//   duration = (double)(finish - start) / CLOCKS_PER_SEC;
//   printf( "%2.1f seconds\n", duration );
//}
//
///* Pauses for a specified number of milliseconds. */
//void sleep( clock_t wait )
//{
//   clock_t goal;
//   goal = wait + clock();
//   while( goal > clock() )
//      ;
//}


// SECTION: A Timer Measuring Real-Time
// ========================================================================
// 
// DEFINITION
// 
// A timer `t' of type Real_timer is an object with a state. It is either
// running or it is stopped. The state is controlled with `t.start()'
// and `t.stop()'. The timer counts the time elapsed since its creation
// or last reset. It counts only the time where it is in the running
// state. The time information is given in seconds.

class Real_timer {
private:
	clock_t	T1;
	double T2;

    double      elapsed;
    double      started;
    int         interv;
    bool        running;
	
    static bool m_failed;

    double   get_real_time()     const; // in seconds
    double   compute_precision() const; // in seconds
public:
    Real_timer();

    void     start();
    void     stop ();
    void     reset();
    void     top(std::string Texte);
    bool     is_running() const { return running; }

    double   time()       const;
    int      intervals()  const { return interv; }
    double   precision()  const;
    // Returns timer precison. Computes it dynamically at first call.
    // Returns -1.0 if timer system call fails, which, for a proper coded
    // test towards precision leads to an immediate stop of an otherwise 
    // infinite loop (fixed tolerance * total time >= precision).
	//double   max()        const { return DBL_MAX; }
};


// -----------------------------------------------------------------------

//Real_timer::Real_timer() : T1(0), T2(0), elapsed(0.0), started(0.0), interv(0), running(true)
//{
//	T1 = clock();
//}
//
//
//// Member functions for Real_timer
//// ===========================
//
//void Real_timer::start() {
//    //started = get_real_time();
//    T1=clock();
//	running = true;
//    ++ interv;
//}
//
//void Real_timer::stop() {
//    //CGAL_precondition( running);
//    //double t = get_real_time();
//    //elapsed += (t - started);
//	T2 = difftime(clock(), T1);
//    started = 0.0;
//    T1=0;
//	running = false;
//}
//
//void Real_timer::reset() {
//    interv  = 0;
//    elapsed = 0.0;
//	T2=0;
//    if (running) {
//	//started = get_real_time();
//	T1=clock();
//	++ interv;
//    } else {
//        started = 0.0;
//		T1=0;
//    }
//}
//
//double Real_timer::time() const {
//    if (running) {
//		clock_t  T3 = clock();
//		return difftime(T3, T1);
//    }
//    return T2;
//}
//
//void	Real_timer::top(string Texte)
//{
//	clock_t  T3 = clock();
//	double r = (double) (T3 - T1)/ CLOCKS_PER_SEC;
//	std::cout << Texte << " .......... " << r << " s" << std::endl;
//	reset();
//}



#endif // bru_REAL_TIMER_H //
// EOF //
