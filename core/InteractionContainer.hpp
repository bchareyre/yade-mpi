// 2004 © Olivier Galizzi <olivier.galizzi@imag.fr>
// 2004 © Janek Kozicki <cosurgi@berlios.de>
// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<boost/thread/mutex.hpp>

#ifdef YADE_OPENMP
	#include<omp.h>
#endif

#include<yade/core/Interaction.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
#  define FOREACH BOOST_FOREACH
#endif

/* This InteractionContainer implementation stores interactions internally in 2 containers:
a std::vector (which allows for const-time linear traversal) and
std::vector of id1 holding std::map of id2 (allowing for fast search by id1,id2). Synchronization
of both is handles by insert & erase methods.

It was originally written by 2008 © Sergei Dorofeenko <sega@users.berlios.de>,
later devirtualized and put here.

Alternative implementations of InteractionContainer should implement the same API. Due to performance
reasons, no base class with virtual methods defining such API programatically is defined (it could
be possible to create class template for this, though).
*/
class InteractionContainer: public Serializable{
	private :
		typedef vector<shared_ptr<Interaction> > ContainerT;
		// linear array of container interactions
		vector<shared_ptr<Interaction> > intrs;
		// array where vecmap[id1] maps id2 to index in intrs (unsigned int)
		vector<map<Body::id_t,size_t> > vecmap;
		// always in sync with intrs.size()
		size_t currSize;
		shared_ptr<Interaction> empty;
		// used only during serialization/deserialization
		vector<shared_ptr<Interaction> > interaction;
	public :
		InteractionContainer(): currSize(0),serializeSorted(false),iterColliderLastRun(-1){
			#ifdef YADE_OPENMP
				threadsPendingErase.resize(omp_get_max_threads());
			#endif
		}
		void clear();
		// iterators
		typedef ContainerT::iterator iterator;
		typedef ContainerT::const_iterator const_iterator;
		iterator begin(){return intrs.begin();}
     	iterator end()  {return intrs.end();}
		const_iterator begin() const {return intrs.begin();}
     	const_iterator end()   const {return intrs.end();}
		// insertion/deletion
		bool insert(Body::id_t id1,Body::id_t id2);
		bool insert(const shared_ptr<Interaction>& i);
		bool erase(Body::id_t id1,Body::id_t id2);
		const shared_ptr<Interaction>& find(Body::id_t id1,Body::id_t id2);
		// index access
		shared_ptr<Interaction>& operator[](size_t id){return intrs[id];}
		const shared_ptr<Interaction>& operator[](size_t id) const { return intrs[id];}
		size_t size(){ return currSize; }
		// simulation API

		//! Erase all non-real (in term of Interaction::isReal()) interactions
		void eraseNonReal();

		// mutual exclusion to avoid crashes in the rendering loop
		boost::mutex drawloopmutex;
		// sort interactions before serializations; useful if comparing XML files from different runs (false by default)
		bool serializeSorted;
		// iteration number when the collider was last run; set by the collider, if it wants interactions that were not encoutered in that step to be deleted by InteractionDispatchers (such as SpatialQuickSortCollider). Other colliders (such as InsertionSortCollider) set it it -1, which is the default
		long iterColliderLastRun;
		//! Ask for erasing the interaction given (from the constitutive law); this resets the interaction (to the initial=potential state) and collider should traverse pendingErase to decide whether to delete the interaction completely or keep it potential
		void requestErase(Body::id_t id1, Body::id_t id2, bool force=false);
		/*! List of pairs of interactions that will be (maybe) erased by the collider; if force==true, they will be deleted unconditionally.
			
			If accessed from within a parallel section, pendingEraseMutex must be locked (this is done inside requestErase for you).

			If there is, at one point, a multi-threaded collider, pendingEraseMutex should be moved to the public part and used from there as well.
		*/
		struct IdsForce{ Body::id_t id1; Body::id_t id2; bool force; };
		#ifdef YADE_OPENMP
			vector<list<IdsForce> > threadsPendingErase;
		#endif
		list<IdsForce> pendingErase;
		/*! Erase all pending interactions unconditionally.

			This should be called only in rare cases that collider is not used but still interactions should be erased.
			Otherwise collider should decide on a case-by-case basis, which interaction to erase for good and which to keep in the potential state
			(without interactionGeometry and interactionPhysics).

			This function doesn't lock pendingEraseMutex, as it is (supposedly) called from no-parallel sections only once per iteration
		*/
		int unconditionalErasePending();

		/*! Clear the list of interaction pending erase: all interactions queued for considering erasing them
		will be dropped; useful for colliders that handle that by themselves, without needing the hint;
		with openMP, it would not be enough to call pendingErase->clear(), this helper function 
		does it for all threads. Use this only if you understand this explanation. */
		void clearPendingErase();

		/*! Traverse all pending interactions and erase them if the (T*)->shouldBeErased(id1,id2) return true
			and keep it if it return false; finally, pendingErase will be clear()'ed.

			Class using this interface (which is presumably a collider) must define the 
					
				bool shouldBeErased(Body::id_t, Body::id_t) const

			method which will be called for every interaction.

			Returns number of interactions, have they been erased or not (this is useful to check if there were some erased, after traversing those)
		*/
		template<class T> int erasePending(const T& t, Scene* rb){
			int ret=0;
			#ifdef YADE_OPENMP
				// shadow the this->pendingErase by the local variable, to share the code
				FOREACH(list<IdsForce>& pendingErase, threadsPendingErase){
			#endif
					FOREACH(const IdsForce& p, pendingErase){
						ret++;
						if(p.force || t.shouldBeErased(p.id1,p.id2,rb)) erase(p.id1,p.id2);
					}
					pendingErase.clear();
			#ifdef YADE_OPENMP
				}
			#endif
			return ret;
		}

	virtual void preProcessAttributes(bool deserializing);
	virtual void postProcessAttributes(bool deserializing);

	REGISTER_ATTRIBUTES(Serializable,(interaction)(serializeSorted));
	REGISTER_CLASS_AND_BASE(InteractionContainer,Serializable);
};
REGISTER_SERIALIZABLE(InteractionContainer);
