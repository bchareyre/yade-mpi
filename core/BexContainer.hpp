// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<string.h>
#include<vector>
#include<Wm3Vector3.h>
// for body_id_t
#include<yade/core/Interaction.hpp>

#ifdef YADE_OPENMP

#include<omp.h>

class BexContainer{
	private:
		typedef std::vector<Vector3r> vvector;
		std::vector<vvector> _forceData;
		std::vector<vvector> _torqueData;
		vvector _force, _torque;
		size_t size;
		int nThreads;
		bool synced;
		boost::mutex globalMutex;

		inline void ensureSize(body_id_t id){
			assert(nThreads>omp_get_thread_num());
			if (size<=(size_t)id) resize(min((size_t)1.5*(id+100),(size_t)(id+2000)));
		}

		inline void ensureSynced(){ if(!synced) throw runtime_error("BexContainer not thread-synchronized; call sync() first!"); }

		/*! Function to allow friend classes to get force even if not synced.
		* Dangerous! The caller must know what it is doing! */
		const Vector3r& getForceUnsynced (body_id_t id){ensureSize(id); return _force[id];}
		const Vector3r& getTorqueUnsynced(body_id_t id){ensureSize(id); return _force[id];}
		friend class PhysicalActionDamperUnit;
	public:
		BexContainer(): size(0), synced(true),syncCount(0){
			nThreads=omp_get_max_threads();
			for(int i=0; i<nThreads; i++){
				_forceData.push_back(vvector()); _torqueData.push_back(vvector());
			}
		}

		/* To be benchmarked: sum thread data in getForce/getTorque upon request for each body individually instead of by the sync() function globally */
		const Vector3r& getForce(body_id_t id)         { ensureSize(id); ensureSynced(); return _force[id]; }
		void  addForce(body_id_t id, const Vector3r& f){ ensureSize(id); synced=false;   _forceData[omp_get_thread_num()][id]+=f;}
		const Vector3r& getTorque(body_id_t id)        { ensureSize(id); ensureSynced(); return _torque[id]; }
		void addTorque(body_id_t id, const Vector3r& f){ ensureSize(id); synced=false;   _torqueData[omp_get_thread_num()][id]+=f;}

		/* Sum contributions from all threads, save to the 0th thread storage.
		 * Locks globalMutex, since one thread modifies other threads' data.
		 * Must be called before get* methods are used. Exception is thrown otherwise, since data are not consistent.
		 */
		inline void sync(){
			if(synced) return;
			boost::mutex::scoped_lock lock(globalMutex);
			if(synced) return; // if synced meanwhile
			// #pragma omp parallel for schedule(static)
			for(long id=0; id<(long)size; id++){
				Vector3r sumF(Vector3r::ZERO), sumT(Vector3r::ZERO);
				for(int thread=0; thread<nThreads; thread++){ sumF+=_forceData[thread][id]; sumT+=_torqueData[thread][id];}
				_force[id]=sumF; _torque[id]=sumT;
			}
			synced=true; syncCount++;
		}
		unsigned long syncCount; 

		/* Change size of containers (number of bodies).
		 * Locks globalMutex, since on threads modifies other threads' data.
		 * Called very rarely (a few times at the beginning of the simulation)
		 */
		void resize(size_t newSize){
			boost::mutex::scoped_lock lock(globalMutex);
			if(size>=newSize) return; // in case on thread was waiting for resize, but it was already satisfied by another one
			for(int thread=0; thread<nThreads; thread++){
				_forceData [thread].resize(newSize);
				_torqueData[thread].resize(newSize);
			}
			_force.resize(newSize); _torque.resize(newSize);
			size=newSize;
		}

		void reset(){
			for(int thread=0; thread<nThreads; thread++){
				memset(_forceData [thread][0], 0,sizeof(Vector3r)*size);
				memset(_torqueData[thread][0],0,sizeof(Vector3r)*size);
			}
			memset(_force [0], 0,sizeof(Vector3r)*size); memset(_torque[0], 0,sizeof(Vector3r)*size);
			synced=true;
		}
};

#else
/* Container for Body External Variables (bex), typically forces and torques from interactions.
 * Values should be reset at every iteration by BexResetter.
 * If you want to add your own bex, there are 4 steps:
 *
 * 	1. Create storage vector
 * 	2. Create accessor function
 * 	3. Update the resize function
 * 	4. Update the reset function
 *
 */
class BexContainer {
	private:
		std::vector<Vector3r> _force;
		std::vector<Vector3r> _torque;
		size_t size;
		inline void ensureSize(body_id_t id){ if(size<=(size_t)id) resize(min((size_t)1.5*(id+100),(size_t)(id+2000)));}
		friend class PhysicalActionDamperUnit;
		const Vector3r& getForceUnsynced (body_id_t id){ return getForce(id);}
		const Vector3r& getTorqueUnsynced(body_id_t id){ return getForce(id);}
	public:
		BexContainer(): size(0),syncCount(0){}
		const Vector3r& getForce(body_id_t id){ensureSize(id); return _force[id];}
		void  addForce(body_id_t id,const Vector3r& f){ensureSize(id); _force[id]+=f;}
		const Vector3r& getTorque(body_id_t id){ensureSize(id); return _torque[id];}
		void  addTorque(body_id_t id,const Vector3r& t){ensureSize(id); _torque[id]+=t;}
		//! Set all bex's to zero
		void reset(){
			memset(_force[0], 0,sizeof(Vector3r)*size);
			memset(_torque[0],0,sizeof(Vector3r)*size);
		}
		//! No-op for API compatibility with the threaded version
		void sync(){return;}
		unsigned long syncCount;
		/*! Resize the container; this happens automatically,
		 * but you may want to set the size beforehand to avoid resizes as the simulation grows. */
		void resize(size_t newSize){
			_force.resize(newSize);
			_torque.resize(newSize);
			size=newSize;
			// std::cerr<<"[DEBUG] BexContainer: Resized to "<<size<<std::endl;
		}
};


#endif
