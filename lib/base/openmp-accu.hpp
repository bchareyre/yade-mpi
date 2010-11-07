// 2010 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once 

// for ZeroInitializer template
#include<yade/lib/base/Math.hpp>

#include<boost/serialization/split_free.hpp>
#include<boost/lexical_cast.hpp>
#include<string>

#ifdef YADE_OPENMP
#include"omp.h"
#include<cstdlib>
#include<unistd.h>
#include<vector>
#include<stdexcept>
#include<iostream>

// O(1) access container which stores data in contiguous chunks of memory
// each chunk belonging to one thread
template<typename T>
class OpenMPArrayAccumulator{
	int CLS;
	size_t nThreads;
	int perCL; // number of elements fitting inside cache line
	std::vector<T*> chunks; // array with pointers to the chunks of memory we have allocated; each item for one thread
	size_t sz; // current number of elements
	size_t nCL; // current number of allocated cache lines
	int nCL_for_N(size_t n){ return n/perCL+(n%perCL==0 ? 0 : 1); } // return number of cache lines to allocate for given number of elements
	public:
		OpenMPArrayAccumulator()        : CLS(sysconf(_SC_LEVEL1_DCACHE_LINESIZE)), nThreads(omp_get_max_threads()), perCL(CLS/sizeof(T)), chunks(nThreads,NULL), sz(0), nCL(0) { }
		OpenMPArrayAccumulator(size_t n): CLS(sysconf(_SC_LEVEL1_DCACHE_LINESIZE)), nThreads(omp_get_max_threads()), perCL(CLS/sizeof(T)), chunks(nThreads,NULL), sz(0), nCL(0) { resize(n); }
		// change number of elements
		void resize(size_t n){
			if(n==sz) return; // nothing to do
			size_t nCL_new=nCL_for_N(n);
			if(nCL_new>nCL){
				for(size_t th=0; th<nThreads; th++){
					void* oldChunk=(void*)chunks[th];
					// FIXME: currently we allocate 4× the memory necessary, otherwise there is crash when accessing past its half -- why?? (http://www.abclinuxu.cz/poradna/programovani/show/318324)
					int succ=posix_memalign((void**)(&chunks[th]),/*alignment*/CLS,/*size*/ nCL_new*CLS);
					if(succ!=0) throw std::runtime_error("OpenMPArrayAccumulator: posix_memalign failed to allocate memory.");
					if(oldChunk){ // initialized to NULL initially, that must not be copied and freed
						memcpy(/*dest*/(void*)chunks[th],/*src*/oldChunk,nCL*CLS); // preserve old data
						free(oldChunk); // deallocate old storage
					}
					nCL=nCL_new;
				}
			}
			// if nCL_new<nCL, do not deallocate memory
			// if nCL_new==nCL, only update sz
			// reset items that were added
			for(size_t s=sz; s>n; s++){ for(size_t th=0; th<nThreads; th++) chunks[th][s]=0; }
			sz=n;
		}
		// clear (does not deallocate storage, anyway)
		void clear() { resize(0); }
		// return number of elements
		size_t size() const { return sz; }
		// get value of one element, by summing contributions of all threads
		T operator[](size_t ix) const { return get(ix); }
		T get(size_t ix) const { T ret(ZeroInitializer<T>()); for(size_t th=0; th<nThreads; th++) ret+=chunks[th][ix]; return ret; }
		// set value of one element; all threads are reset except for the 0th one, which assumes that value
		void set(size_t ix, const T& val){ for(size_t th=0; th<nThreads; th++) chunks[th][ix]=(th==0?val:ZeroInitializer<T>()); }
		// reset one element to ZeroInitializer
		void add(size_t ix, const T& diff){ chunks[omp_get_thread_num()][ix]+=diff; }
		void reset(size_t ix){ set(ix,ZeroInitializer<T>()); }
		// fill all memory with zeros; the caller is responsible for assuring that such value is meaningful when converted to T
		// void memsetZero(){ for(size_t th=0; th<nThreads; th++) memset(&chunks[th],0,CLS*nCL); }
};

/* Class accumulating results of type T in parallel sections. Summary value (over all threads) can be read or reset in non-parallel sections only.

#. update value, useing the += operator.
#. Get value using implicit conversion to T (in non-parallel sections only)
#. Reset value by calling reset() (in non-parallel sections only)

Storage of data is aligned to cache line size, no false sharing should occur (but some space is wasted, OTOH)
This will currently not compile for non-POSIX systems, as we use sysconf and posix_memalign.

*/
template<typename T>
class OpenMPAccumulator{
		int CLS; // cache line size
		int nThreads;
		int eSize; // size of an element, computed from cache line size and sizeof(T)
		char* data; // use void* rather than T*, since with T* the pointer arithmetics has sizeof(T) as unit, which is confusing; char* takes one byte
	public:
	// initialize storage with _zeroValue, depending on muber of threads
	OpenMPAccumulator(): CLS(sysconf(_SC_LEVEL1_DCACHE_LINESIZE)), nThreads(omp_get_max_threads()), eSize(CLS*(sizeof(T)/CLS+(sizeof(T)%CLS==0 ? 0 :1))) {
		int succ=posix_memalign(/*where allocated*/(void**)&data,/*alignment*/CLS,/*size*/ nThreads*eSize);
		if(succ!=0) throw std::runtime_error("OpenMPAccumulator: posix_memalign failed to allocate memory.");
		reset();
	}
	~OpenMPAccumulator() { free((void*)data); }
	// lock-free addition
	void operator+=(const T& val){ *((T*)(data+omp_get_thread_num()*eSize))+=val; }
	// return summary value; must not be used concurrently
	operator T() const { return get(); }
	// reset to zeroValue; must NOT be used concurrently
	void reset(){ for(int i=0; i<nThreads; i++) *(T*)(data+i*eSize)=ZeroInitializer<T>(); }
	// this can be used to get the value from python, something like
	// .def_readonly("myAccu",&OpenMPAccumulator::get,"documentation")
	T get() const { T ret(ZeroInitializer<T>()); for(int i=0; i<nThreads; i++) ret+=*(T*)(data+i*eSize); return ret; }
	void set(const T& value){ reset(); /* set value for the 0th thread */ *(T*)(data)=value; }
};
#else 
template<typename T>
class OpenMPArrayAccumulator{
	std::vector<T> data;
	public:
		OpenMPArrayAccumulator(){}
		OpenMPArrayAccumulator(size_t n){ resize(n); }
		void resize(size_t s){ data.resize(s,ZeroInitializer<T>()); }
		void clear(){ data.clear(); }
		size_t size() const { return data.size(); }
		T operator[](size_t ix) const { return get(ix); }
		T get(size_t ix) const { return data[ix]; }
		void add (size_t ix, const T& diff){ data[ix]+=diff; }
		void set(size_t ix, const T& val){ data[ix]=val; }
		void reset(size_t ix){ data[ix]=ZeroInitializer<T>(); }
};

// single-threaded version of the accumulator above
template<typename T>
class OpenMPAccumulator{
	T data;
public:
	void operator+=(const T& val){ data+=val; }
	operator T() const { return get(); }
	void reset(){ data=ZeroInitializer<T>(); }
	T get() const { return data; }
	void set(const T& val){ data=val; }
};
#endif

// boost serialization
	BOOST_SERIALIZATION_SPLIT_FREE(OpenMPAccumulator<int>);
	template<class Archive> void save(Archive &ar, const OpenMPAccumulator<int>& a, unsigned int version){ int value=a.get(); ar & BOOST_SERIALIZATION_NVP(value); }
	template<class Archive> void load(Archive &ar,       OpenMPAccumulator<int>& a, unsigned int version){ int value; ar & BOOST_SERIALIZATION_NVP(value); a.set(value); }
	BOOST_SERIALIZATION_SPLIT_FREE(OpenMPAccumulator<Real>);
	template<class Archive> void save(Archive &ar, const OpenMPAccumulator<Real>& a, unsigned int version){ Real value=a.get(); ar & BOOST_SERIALIZATION_NVP(value); }
	template<class Archive> void load(Archive &ar,       OpenMPAccumulator<Real>& a, unsigned int version){ Real value; ar & BOOST_SERIALIZATION_NVP(value); a.set(value); }
	BOOST_SERIALIZATION_SPLIT_FREE(OpenMPArrayAccumulator<Real>);
	template<class Archive> void save(Archive &ar, const OpenMPArrayAccumulator<Real>& a, unsigned int version){ size_t size=a.size(); ar & BOOST_SERIALIZATION_NVP(size); for(size_t i=0; i<size; i++) { Real item(a.get(i)); ar & boost::serialization::make_nvp(("item"+boost::lexical_cast<std::string>(i)).c_str(),item); } }
	template<class Archive> void load(Archive &ar,       OpenMPArrayAccumulator<Real>& a, unsigned int version){ size_t size; ar & BOOST_SERIALIZATION_NVP(size); a.resize(size); for(size_t i=0; i<size; i++){ Real item; ar & boost::serialization::make_nvp(("item"+boost::lexical_cast<std::string>(i)).c_str(),item); a.set(i,item); } }
