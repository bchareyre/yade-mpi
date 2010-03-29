// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#ifdef YADE_OPENMP
#include"omp.h"
/* Class accumulating results of type T in parallel sections. Summary value (over all threads) can be read or reset in non-parallel sections only.

#. update value, useing the += operator.
#. Get value using implicit conversion to T (in non-parallel sections only)
#. Reset value by calling reset() (in non-parallel sections only)

*/
template<typename T, T* zeroValue>
class OpenMPAccumulator{
		const int nThreads;
		std::vector<T> data;
	public:
	// initialize storage with _zeroValue, depending on muber of threads
	OpenMPAccumulator(): nThreads(omp_get_max_threads()), data(nThreads,*zeroValue) {}
	// lock-free addition
	void operator+=(const T& val){ data[omp_get_thread_num()]+=val; }
	// return summary value; must not be used concurrently
	operator T(){ T ret(*zeroValue); for(int i=0; i<nThreads; i++) ret+=data[i]; return ret; }
	// reset to zeroValue; must NOT be used concurrently
	void reset(){ for(int i=0; i<nThreads; i++) data[i]=*zeroValue; }
};
#else 
// single-threaded version of the accumulator above
template<typename T, T* zeroValue>
class OpenMPAccumulator{
	T data;
public:
	void operator+=(const T& val){ data+=val; }
	operator T(){ return data; }
	void reset(){ data=*zeroValue; }
};
#endif
