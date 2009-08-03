// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<string.h>
#include<vector>
#include<Wm3Vector3.h>
// for body_id_t
#include<yade/core/Interaction.hpp>

#ifdef YADE_OPENMP

#include<omp.h>
/*! Container for Body External Variables (bex), typically forces and torques from interactions.
 * Values should be reset at every iteration by calling BexContainer::reset();
 * If you want to add your own bex, you need to:
 *
 * 	1. Create storage vector
 * 	2. Create accessor function
 * 	3. Update the resize function
 * 	4. Update the reset function
 * 	5. update the sync function (for the multithreaded implementation)
 *
 * This class exists in two flavors: non-parallel and parallel. The parallel one stores
 * bex increments separately for every thread and sums those when sync() is called.
 * The reason of this design is that the container is not truly random-access, but rather
 * is written to everywhere in one phase and read in the next one. Adding to force/torque
 * marks the container as dirty and sync() must be performed before reading the stored data.
 * Calling getForce/getTorque when the container is not synchronized throws an exception.
 *
 * It is intentional that sync() needs to be called exlicitly, since syncs are expensive and
 * the programmer should be aware of that. Sync is however performed only if the container
 * is dirty. Every full sync increments the syncCount variable, that should ideally equal
 * the number of steps (one per step).
 *
 * The number of threads (omp_get_max_threads) may not change once BexContainer is constructed.
 *
 * The non-parallel flavor has the same interface, but sync() is no-op and synchronization
 * is not enforced at all.
 */

//! This is the parallel flavor of BexContainer
class BexContainer{
	private:
		typedef std::vector<Vector3r> vvector;
		std::vector<vvector> _forceData;
		std::vector<vvector> _torqueData;
		std::vector<vvector> _moveData;
		std::vector<vvector> _rotData;
		vvector _force, _torque, _move, _rot;
		size_t size;
		int nThreads;
		bool synced,moveRotUsed;
		boost::mutex globalMutex;

		inline void ensureSize(body_id_t id){
			assert(nThreads>omp_get_thread_num());
			if (size<=(size_t)id) resize(min((size_t)1.5*(id+100),(size_t)(id+2000)));
		}

		inline void ensureSynced(){ if(!synced) throw runtime_error("BexContainer not thread-synchronized; call sync() first!"); }

		/*! Function to allow friend classes to get force even if not synced.
		* Dangerous! The caller must know what it is doing! (i.e. don't read after write
		* for a particular body id. */
		const Vector3r& getForceUnsynced (body_id_t id){ensureSize(id); return _force[id];}
		const Vector3r& getTorqueUnsynced(body_id_t id){ensureSize(id); return _force[id];}
		friend class PhysicalActionDamperUnit;
	public:
		BexContainer(): size(0), synced(true),moveRotUsed(false),syncCount(0){
			nThreads=omp_get_max_threads();
			for(int i=0; i<nThreads; i++){
				_forceData.push_back(vvector()); _torqueData.push_back(vvector());
				_moveData.push_back(vvector()); _rotData.push_back(vvector());
			}
		}

		/* To be benchmarked: sum thread data in getForce/getTorque upon request for each body individually instead of by the sync() function globally */
		const Vector3r& getForce(body_id_t id)         { ensureSize(id); ensureSynced(); return _force[id]; }
		void  addForce(body_id_t id, const Vector3r& f){ ensureSize(id); synced=false;   _forceData[omp_get_thread_num()][id]+=f;}
		const Vector3r& getTorque(body_id_t id)        { ensureSize(id); ensureSynced(); return _torque[id]; }
		void addTorque(body_id_t id, const Vector3r& t){ ensureSize(id); synced=false;   _torqueData[omp_get_thread_num()][id]+=t;}
		const Vector3r& getMove(body_id_t id)          { ensureSize(id); ensureSynced(); return _move[id]; }
		void  addMove(body_id_t id, const Vector3r& m) { ensureSize(id); synced=false; moveRotUsed=true; _moveData[omp_get_thread_num()][id]+=m;}
		const Vector3r& getRot(body_id_t id)           { ensureSize(id); ensureSynced(); return _rot[id]; }
		void  addRot(body_id_t id, const Vector3r& r)  { ensureSize(id); synced=false; moveRotUsed=true; _rotData[omp_get_thread_num()][id]+=r;}

		/* Sum contributions from all threads, save to _force&_torque.
		 * Locks globalMutex, since one thread modifies common data (_force&_torque).
		 * Must be called before get* methods are used. Exception is thrown otherwise, since data are not consistent. */
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
			if(moveRotUsed){
				for(long id=0; id<(long)size; id++){
					Vector3r sumM(Vector3r::ZERO), sumR(Vector3r::ZERO);
					for(int thread=0; thread<nThreads; thread++){ sumM+=_moveData[thread][id]; sumR+=_rotData[thread][id];}
					_move[id]=sumM; _rot[id]=sumR;
				}
			}
			synced=true; syncCount++;
		}
		unsigned long syncCount; 

		/* Change size of containers (number of bodies).
		 * Locks globalMutex, since on threads modifies other threads' data.
		 * Called very rarely (a few times at the beginning of the simulation). */
		void resize(size_t newSize){
			boost::mutex::scoped_lock lock(globalMutex);
			if(size>=newSize) return; // in case on thread was waiting for resize, but it was already satisfied by another one
			for(int thread=0; thread<nThreads; thread++){
				_forceData [thread].resize(newSize,Vector3r::ZERO);
				_torqueData[thread].resize(newSize,Vector3r::ZERO);
				_moveData[thread].resize(newSize,Vector3r::ZERO);
				_rotData[thread].resize(newSize,Vector3r::ZERO);
			}
			_force.resize(newSize,Vector3r::ZERO); _torque.resize(newSize,Vector3r::ZERO);
			_move.resize(newSize,Vector3r::ZERO); _rot.resize(newSize,Vector3r::ZERO);
			size=newSize;
		}
		/*! Reset all data, also reset summary forces/torques and mark the container clean. */
		// perhaps should be private and friend MetaBody or whatever the only caller should be
		void reset(){
			for(int thread=0; thread<nThreads; thread++){
				memset(_forceData [thread][0],0,sizeof(Vector3r)*size);
				memset(_torqueData[thread][0],0,sizeof(Vector3r)*size);
				if(moveRotUsed){
					memset(_moveData  [thread][0],0,sizeof(Vector3r)*size);
					memset(_rotData   [thread][0],0,sizeof(Vector3r)*size);
				}
			}
			memset(_force [0], 0,sizeof(Vector3r)*size);
			memset(_torque[0], 0,sizeof(Vector3r)*size);
			if(moveRotUsed){
				memset(_move  [0], 0,sizeof(Vector3r)*size);
				memset(_rot   [0], 0,sizeof(Vector3r)*size);
			}
			synced=true; moveRotUsed=false;
		}
		//! say for how many threads we have allocated space
		const int& getNumAllocatedThreads() const {return nThreads;}
		const bool& getMoveRotUsed() const {return moveRotUsed;}
};

#else
//! This is the non-parallel flavor of BexContainer
class BexContainer {
	private:
		std::vector<Vector3r> _force;
		std::vector<Vector3r> _torque;
		std::vector<Vector3r> _move;
		std::vector<Vector3r> _rot;
		size_t size;
		inline void ensureSize(body_id_t id){ if(size<=(size_t)id) resize(min((size_t)1.5*(id+100),(size_t)(id+2000)));}
		friend class PhysicalActionDamperUnit;
		const Vector3r& getForceUnsynced (body_id_t id){ return getForce(id);}
		const Vector3r& getTorqueUnsynced(body_id_t id){ return getForce(id);}
		bool moveRotUsed;
	public:
		BexContainer(): size(0), moveRotUsed(false), syncCount(0){}
		const Vector3r& getForce(body_id_t id){ensureSize(id); return _force[id];}
		void  addForce(body_id_t id,const Vector3r& f){ensureSize(id); _force[id]+=f;}
		const Vector3r& getTorque(body_id_t id){ensureSize(id); return _torque[id];}
		void  addTorque(body_id_t id,const Vector3r& t){ensureSize(id); _torque[id]+=t;}
		const Vector3r& getMove(body_id_t id){ensureSize(id); return _move[id];}
		void  addMove(body_id_t id,const Vector3r& f){ensureSize(id); moveRotUsed=true; _move[id]+=f;}
		const Vector3r& getRot(body_id_t id){ensureSize(id); return _rot[id];}
		void  addRot(body_id_t id,const Vector3r& f){ensureSize(id); moveRotUsed=true; _rot[id]+=f;}
		//! Set all bex's to zero
		void reset(){
			memset(_force [0],0,sizeof(Vector3r)*size);
			memset(_torque[0],0,sizeof(Vector3r)*size);
			if(moveRotUsed){
				memset(_move  [0],0,sizeof(Vector3r)*size);
				memset(_rot   [0],0,sizeof(Vector3r)*size);
				moveRotUsed=False;
			}
		}
		//! No-op for API compatibility with the threaded version
		void sync(){return;}
		unsigned long syncCount;
		/*! Resize the container; this happens automatically,
		 * but you may want to set the size beforehand to avoid resizes as the simulation grows. */
		void resize(size_t newSize){
			_force.resize(newSize,Vector3r::ZERO);
			_torque.resize(newSize,Vector3r::ZERO);
			_move.resize(newSize,Vector3r::ZERO);
			_rot.resize(newSize,Vector3r::ZERO);
			size=newSize;
		}
		const int getNumAllocatedThreads() const {return 1;}
		const bool& getMoveRotUsed() const {return moveRotUsed;}
};


#endif
