// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<lib/base/Math.hpp>
#include<core/Body.hpp>

#include<boost/static_assert.hpp>
// make sure that (void*)&vec[0]==(void*)&vec
BOOST_STATIC_ASSERT(sizeof(Vector3r)==3*sizeof(Real));


#ifdef YADE_OPENMP

#include<omp.h>
/*! Container for Body External Variables (forces), typically forces and torques from interactions.
 * Values should be reset at every iteration by calling ForceContainer::reset();
 * If you want to add your own force type, you need to:
 *
 * 	1. Create storage vector
 * 	2. Create accessor function
 * 	3. Update the resize function
 * 	4. Update the reset function
 * 	5. update the sync function (for the multithreaded implementation)
 *
 * This class exists in two flavors: non-parallel and parallel. The parallel one stores
 * force increments separately for every thread and sums those when sync() is called.
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
 * The number of threads (omp_get_max_threads) may not change once ForceContainer is constructed.
 *
 * The non-parallel flavor has the same interface, but sync() is no-op and synchronization
 * is not enforced at all.
 */

//! This is the parallel flavor of ForceContainer
class ForceContainer{
	private:
		typedef std::vector<Vector3r> vvector;
		std::vector<vvector> _forceData;
		std::vector<vvector> _torqueData;
		std::vector<vvector> _moveData;
		std::vector<vvector> _rotData;
		std::vector<Body::id_t>  _maxId;
		vvector _force, _torque, _move, _rot, _permForce, _permTorque;
		std::vector<size_t> sizeOfThreads;
		size_t size = 0;
		bool syncedSizes = true;
		int nThreads;
		bool synced = true;
		bool moveRotUsed = false;
		bool permForceUsed = false;
		boost::mutex globalMutex;
		Vector3r _zero = Vector3r::Zero();

		void ensureSize(Body::id_t id, int threadN);
		void ensureSynced();
		
		// dummy function to avoid template resolution failure
		friend class boost::serialization::access;
		template<class ArchiveT> void serialize(ArchiveT & ar, unsigned int version){}
	public:
		unsigned long syncCount = 0;
		long lastReset = 0;
		ForceContainer();
		const Vector3r& getForce(Body::id_t id);
		void  addForce(Body::id_t id, const Vector3r& f);
		const Vector3r& getTorque(Body::id_t id);
		void  addTorque(Body::id_t id, const Vector3r& t);
		const Vector3r& getMove(Body::id_t id);
		void  addMove(Body::id_t id, const Vector3r& m);
		const Vector3r& getRot(Body::id_t id);
		void  addRot(Body::id_t id, const Vector3r& r);
		void  addMaxId(Body::id_t id);

		void  setPermForce(Body::id_t id, const Vector3r& f);
		void  setPermTorque(Body::id_t id, const Vector3r& t);
		const Vector3r& getPermForce(Body::id_t id);
		const Vector3r& getPermTorque(Body::id_t id);
		
		/*! Function to allow friend classes to get force even if not synced. Used for clumps by NewtonIntegrator.
		* Dangerous! The caller must know what it is doing! (i.e. don't read after write
		* for a particular body id. */
		const Vector3r& getForceUnsynced (Body::id_t id);
		const Vector3r& getTorqueUnsynced(Body::id_t id);
		void  addForceUnsynced(Body::id_t id, const Vector3r& f);
		void  addTorqueUnsynced(Body::id_t id, const Vector3r& m);
		
		/* To be benchmarked: sum thread data in getForce/getTorque upon request for each body individually instead of by the sync() function globally */
		// this function is used from python so that running simulation is not slowed down by sync'ing on occasions
		// since Vector3r writes are not atomic, it might (rarely) return wrong value, if the computation is running meanwhile
		Vector3r getForceSingle (Body::id_t id);
		Vector3r getTorqueSingle(Body::id_t id);
		Vector3r getMoveSingle  (Body::id_t id);
		Vector3r getRotSingle   (Body::id_t id);
		
		void syncSizesOfContainers();
		/* Sum contributions from all threads, save to _force&_torque.
		 * Locks globalMutex, since one thread modifies common data (_force&_torque).
		 * Must be called before get* methods are used. Exception is thrown otherwise, since data are not consistent. */
		void sync();
		void resize(size_t newSize, int threadN);
		void resizePerm(size_t newSize);
		/*! Reset all resetable data, also reset summary forces/torques and mark the container clean.
		If resetAll, reset also user defined forces and torques*/
		// perhaps should be private and friend Scene or whatever the only caller should be
		void reset(long iter, bool resetAll=false);
		//! say for how many threads we have allocated space
		const int& getNumAllocatedThreads();
		const bool& getMoveRotUsed();
		const bool& getPermForceUsed();
};

#else
//! This is the non-parallel flavor of ForceContainer
class ForceContainer {
	private:
		std::vector<Vector3r> _force;
		std::vector<Vector3r> _torque;
		std::vector<Vector3r> _move;
		std::vector<Vector3r> _rot;
		std::vector<Vector3r> _permForce;
		std::vector<Vector3r> _permTorque;
		Body::id_t _maxId=0;
		size_t size=0;
		bool moveRotUsed = false;
		bool permForceUsed = false;
		void ensureSize(Body::id_t id);
		
		const Vector3r& getForceUnsynced (Body::id_t id);
		const Vector3r& getTorqueUnsynced(Body::id_t id);
		
		// dummy function to avoid template resolution failure
		friend class boost::serialization::access; template<class ArchiveT> void serialize(ArchiveT & ar, unsigned int version){}
	public:
		unsigned long syncCount = 0;
		long lastReset=0;
		ForceContainer() {}
		const Vector3r& getForce(Body::id_t id);
		void  addForce(Body::id_t id,const Vector3r& f);
		const Vector3r& getTorque(Body::id_t id);
		void  addTorque(Body::id_t id,const Vector3r& t);
		const Vector3r& getMove(Body::id_t id);
		void  addMove(Body::id_t id,const Vector3r& f);
		const Vector3r& getRot(Body::id_t id);
		void  addRot(Body::id_t id,const Vector3r& f);
		void  setPermForce(Body::id_t id, const Vector3r& f);
		void setPermTorque(Body::id_t id, const Vector3r& t);
		void  addMaxId(Body::id_t id);
		const Vector3r& getPermForce(Body::id_t id);
		const Vector3r& getPermTorque(Body::id_t id);
		// single getters do the same as globally synced ones in the non-parallel flavor
		const Vector3r getForceSingle (Body::id_t id);
		const Vector3r getTorqueSingle(Body::id_t id);
		const Vector3r& getMoveSingle  (Body::id_t id);
		const Vector3r& getRotSingle   (Body::id_t id);

		//! Set all forces to zero
		void reset(long iter, bool resetAll=false);
		void sync();
		// interaction in which the container was last reset; used by NewtonIntegrator to detect whether ForceResetter was not forgotten
		/*! Resize the container; this happens automatically,
		 * but you may want to set the size beforehand to avoid resizes as the simulation grows. */
		void resize(size_t newSize);
		const int getNumAllocatedThreads() const;
		const bool& getMoveRotUsed() const;
		const bool& getPermForceUsed() const;
};

#endif
