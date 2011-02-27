// 2004 © Olivier Galizzi <olivier.galizzi@imag.fr>
// 2004 © Janek Kozicki <cosurgi@berlios.de>
// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/lib/serialization/Serializable.hpp>
#include<boost/thread/mutex.hpp>

#ifdef YADE_OPENMP
	#include<omp.h>
#endif

#include<yade/core/Interaction.hpp>
#include<yade/core/BodyContainer.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
#  define FOREACH BOOST_FOREACH
#endif

/* This InteractionContainer implementation has reference to the body container and
stores interactions in 2 places:

* Internally in a std::vector; that allows for const-time linear traversal.
  Each interaction internally holds back-reference to the position in this container in Interaction::linIx.
* Inside Body::intrs (in the body with min(id1,id2)).

Both must be kep in sync, which is handled by insert & erase methods.

It was originally written by 2008 © Sergei Dorofeenko <sega@users.berlios.de>,
later devirtualized and put here.

Alternative implementations of InteractionContainer should implement the same API. Due to performance
reasons, no base class with virtual methods defining such API programatically is defined (it could
be possible to create class template for this, though).

Future (?):

* The shared_ptr<Interaction> might be duplicated in body id2 as well. That would allow to retrieve
  in a straigthforward manner all interactions with given body id, for instance. Performance implications
  are not clear.

* the linear vector might be removed; in favor of linear traversal of bodies by their subdomains,
  then traversing the map in each body. If the previous point would come to realization, half of the
  interactions would have to be skipped explicitly in such a case.

*/
class InteractionContainer: public Serializable{
	private:
		typedef vector<shared_ptr<Interaction> > ContainerT;
		// linear array of container interactions
		ContainerT linIntrs;
		// pointer to body container, since each body holds (some) interactions
		// this must always point to scene->bodies->body
		const BodyContainer::ContainerT* bodies;
		// always in sync with intrs.size(), to avoid that function call
		size_t currSize;
		shared_ptr<Interaction> empty;
		// used only during serialization/deserialization
		vector<shared_ptr<Interaction> > interaction;
	public:
		// flag for notifying the collider that persistent data should be invalidated
		bool dirty;
		// required by the class factory... :-|
		InteractionContainer(): currSize(0),dirty(false),serializeSorted(false),iterColliderLastRun(-1){
			bodies=NULL;
			#ifdef YADE_OPENMP
				threadsPendingErase.resize(omp_get_max_threads());
			#endif
		}
		void clear();
		// iterators
		typedef ContainerT::iterator iterator;
		typedef ContainerT::const_iterator const_iterator;
		iterator begin(){return linIntrs.begin();}
		iterator end()  {return linIntrs.end();}
		const_iterator begin() const {return linIntrs.begin();}
		const_iterator end()   const {return linIntrs.end();}
		// insertion/deletion
		bool insert(Body::id_t id1,Body::id_t id2);
		bool insert(const shared_ptr<Interaction>& i);
		bool erase(Body::id_t id1,Body::id_t id2);
		const shared_ptr<Interaction>& find(Body::id_t id1,Body::id_t id2);
		// index access
		shared_ptr<Interaction>& operator[](size_t id){return linIntrs[id];}
		const shared_ptr<Interaction>& operator[](size_t id) const { return linIntrs[id];}
		size_t size(){ return currSize; }
		// simulation API

		//! Erase all non-real (in term of Interaction::isReal()) interactions
		void eraseNonReal();

		// mutual exclusion to avoid crashes in the rendering loop
		boost::mutex drawloopmutex;
		// sort interactions before serializations; useful if comparing XML files from different runs (false by default)
		bool serializeSorted;
		// iteration number when the collider was last run; set by the collider, if it wants interactions that were not encoutered in that step to be deleted by InteractionLoop (such as SpatialQuickSortCollider). Other colliders (such as InsertionSortCollider) set it it -1, which is the default
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
			(without geom and phys).

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

	// we must call Scene's ctor (and from Scene::postLoad), since we depend on the existing BodyContainer at that point.
	void postLoad__calledFromScene(const shared_ptr<BodyContainer>&);
	void preLoad(InteractionContainer&);
	void preSave(InteractionContainer&);
	void postSave(InteractionContainer&);


	REGISTER_ATTRIBUTES(Serializable,(interaction)(serializeSorted)(dirty));
	REGISTER_CLASS_AND_BASE(InteractionContainer,Serializable);
};
REGISTER_SERIALIZABLE(InteractionContainer);
