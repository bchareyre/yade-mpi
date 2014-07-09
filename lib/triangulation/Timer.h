#ifndef bru_REAL_TIMER_H
#define bru_REAL_TIMER_H 1

#include <iostream>
#include <time.h>

using std::string;

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



#endif // bru_REAL_TIMER_H //
// EOF //
