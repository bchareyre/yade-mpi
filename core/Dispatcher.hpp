/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<boost/lexical_cast.hpp>

#include<yade/core/Engine.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/Omega.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>

class Dispatcher: public Engine
{
	public :
		typedef list<shared_ptr<Functor> >::iterator FunctorListIterator;
		//! Should be called by the dispatcher before every loop (could be somehow optimized, since it will change very rarely)
		void updateScenePtr() { FOREACH(shared_ptr<Functor> f, functors){ f->scene=scene; } }

		void clear(){ functors.clear(); }
		
		virtual ~Dispatcher();
		
		virtual string getFunctorType() { throw; };
		virtual int getDimension() { throw; };
		virtual string getBaseClassType(unsigned int ) { throw; };

		// emulate postProcessAttributes of real dispatchers that are never otherwise called; this hack will be removed once dispatchers are types on their own
		virtual void Dispatcher_postProcessAttributes_deserializing(){ throw runtime_error("Dispatcher::Dispatcher_postProcessAttributes_deserializing should never be called (must be overridden in Dispatcher1D/Dispatcher2D)"); }
		void postProcessAttributes(bool deserializing){
			if(!deserializing) return;
			Dispatcher_postProcessAttributes_deserializing();
		}

	YADE_CLASS_BASE_DOC_ATTRS(Dispatcher,Engine,"Engine dispatching control to its associated functors, based on types of argument it receives.",
		((list<shared_ptr<Functor> >,functors,,"Instances of functors"))
	);
};
REGISTER_SERIALIZABLE(Dispatcher);


// HELPER MACROS
// supposed to be passed to YADE_CLASS_BASE_DOC_ATTRS_PY in the 5th argument; takes class name as arg
#define YADE_PY_DISPATCHER(DispatcherT) .def("__init__",python::make_constructor(Dispatcher_ctor_list<DispatcherT>),"Construct with list of associated functors.").add_property("functors",&Dispatcher_functors_get<DispatcherT>,&Dispatcher_functors_set<DispatcherT>,"Functors objects associated with this dispatcher.").def("dispMatrix",&DispatcherT::dump,python::arg("names")=true,"Return dictionary with contents of the dispatch matrix.").def("dispFunctor",&DispatcherT::getFunctor,"Return functor that would be dispatched for given argument(s); None if no dispatch; ambiguous dispatch throws.");


// HELPER FUNCTIONS

/*! Function returning class name (as string) for given index and topIndexable (top-level indexable, such as Shape, Material and so on)
This function exists solely for debugging, is quite slow: it has to traverse all classes and ask for inheritance information.
It should be used primarily to convert indices to names in Dispatcher::dictDispatchMatrix?D; since it relies on Omega for RTTI,
this code could not be in Dispatcher itself.
s*/
template<class topIndexable>
std::string Dispatcher_indexToClassName(int idx){
	scoped_ptr<topIndexable> top(new topIndexable);
	std::string topName=top->getClassName();
	typedef std::pair<string,DynlibDescriptor> classItemType;
	FOREACH(classItemType clss, Omega::instance().getDynlibsDescriptor()){
		if(Omega::instance().isInheritingFrom_recursive(clss.first,topName) || clss.first==topName){
			// create instance, to ask for index
			shared_ptr<topIndexable> inst=dynamic_pointer_cast<topIndexable>(ClassFactory::instance().createShared(clss.first));
			assert(inst);
			if(inst->getClassIndex()<0 && inst->getClassName()!=top->getClassName()){
				throw logic_error("Class "+inst->getClassName()+" didn't use REGISTER_CLASS_INDEX("+inst->getClassName()+","+top->getClassName()+") and/or forgot to call createIndex() in the ctor. [[ Please fix that! ]]");
			}
			if(inst->getClassIndex()==idx) return clss.first;
		}
	}
	throw runtime_error("No class with index "+boost::lexical_cast<string>(idx)+" found (top-level indexable is "+topName+")");
}

//! Return class index of given indexable
template<typename TopIndexable>
int Indexable_getClassIndex(const shared_ptr<TopIndexable> i){return i->getClassIndex();}

//! Return sequence (hierarchy) of class indices of given indexable; optionally convert to names
template<typename TopIndexable>
python::list Indexable_getClassIndices(const shared_ptr<TopIndexable> i, bool convertToNames){
	int depth=1; python::list ret; int idx0=i->getClassIndex();
	if(convertToNames) ret.append(Dispatcher_indexToClassName<TopIndexable>(idx0));
	else ret.append(idx0);
	if(idx0<0) return ret; // don't continue and call getBaseClassIndex(), since we are at the top already
	while(true){
		int idx=i->getBaseClassIndex(depth++);
		if(convertToNames) ret.append(Dispatcher_indexToClassName<TopIndexable>(idx));
		else ret.append(idx);
		if(idx<0) return ret;
	}
}



//! Return functors of this dispatcher, as list of functors of appropriate type
template<typename DispatcherT>
std::vector<shared_ptr<typename DispatcherT::functorType> > Dispatcher_functors_get(shared_ptr<DispatcherT> self){
	std::vector<shared_ptr<typename DispatcherT::functorType> > ret;
	FOREACH(const shared_ptr<Functor>& functor, self->functors){ shared_ptr<typename DispatcherT::functorType> functorRightType(dynamic_pointer_cast<typename DispatcherT::functorType>(functor)); if(!functorRightType) throw logic_error("Internal error: Dispatcher of type "+self->getClassName()+" did not contain Functor of the required type "+typeid(typename DispatcherT::functorType).name()+"?"); ret.push_back(functorRightType); }
	return ret;
}

template<typename DispatcherT>
void Dispatcher_functors_set(shared_ptr<DispatcherT> self, std::vector<shared_ptr<typename DispatcherT::functorType> > functors){
	self->clear();
	FOREACH(const shared_ptr<typename DispatcherT::functorType>& item, functors) self->add(item);
}

// Dispatcher is not a template, hence converting this into a real constructor would be complicated; keep it separated, at least for now...
//! Create dispatcher of given type, with functors given as list in argument
template<typename DispatcherT>
shared_ptr<DispatcherT> Dispatcher_ctor_list(const std::vector<shared_ptr<typename DispatcherT::functorType> >& functors){
	shared_ptr<DispatcherT> instance(new DispatcherT);
	Dispatcher_functors_set<DispatcherT>(instance,functors);
	return instance;
}



template
<
	class baseClass,
	class FunctorType,
	class FunctorReturnType,
	class FunctorArguments,
	bool autoSymmetry=true
>
class Dispatcher1D : public Dispatcher,
				public DynLibDispatcher
				<	  TYPELIST_1(baseClass)		// base classes for dispatch
					, FunctorType		// class that provides multivirtual call
					, FunctorReturnType		// return type
					, FunctorArguments
					, autoSymmetry
				>
{

	public :
		typedef baseClass argType1;
		typedef FunctorType functorType;
		typedef DynLibDispatcher<TYPELIST_1(baseClass),FunctorType,FunctorReturnType,FunctorArguments,autoSymmetry> dispatcherBase;

		shared_ptr<FunctorType> getFunctor(shared_ptr<baseClass> arg){ return getExecutor(arg); }
		python::dict dump(bool convertIndicesToNames){
			python::dict ret;
			FOREACH(const DynLibDispatcher_Item1D& item, dispatcherBase::dataDispatchMatrix1D()){
				if(convertIndicesToNames){
					string arg1=Dispatcher_indexToClassName<argType1>(item.ix1);
					ret[python::make_tuple(arg1)]=item.functorName;
				} else ret[python::make_tuple(item.ix1)]=item.functorName;
			}
			return ret;
		}
		virtual void add(FunctorType* eu){ add(shared_ptr<FunctorType>(eu)); }
		virtual void add(shared_ptr<FunctorType> eu){
			bool dupe=false; string eun=eu->getClassName();
			FOREACH(const shared_ptr<Functor>& f, functors) { if(eun==f->getClassName()) dupe=true; }
			if(!dupe) functors.push_back(eu);
			add1DEntry(eu->get1DFunctorType1(),eu);
		}

		int getDimension() { return 1; }
	
		virtual string getFunctorType(){
			shared_ptr<FunctorType> eu(new FunctorType);
			return eu->getClassName();
		}

		virtual string getBaseClassType(unsigned int i){
			if (i==0) { shared_ptr<baseClass> bc(new baseClass); return bc->getClassName(); }
			else return "";
		}


		virtual void Dispatcher_postProcessAttributes_deserializing(){ Dispatcher1D::postProcessAttributes(true); }
		// never gets called directly, but via the proxy above (hack)
		void postProcessAttributes(bool deserializing){
			if(!deserializing) return;
			FOREACH(shared_ptr<Functor> f, functors) add(static_pointer_cast<FunctorType>(f));
		}

	public:
	REGISTER_ATTRIBUTES(Dispatcher,);
	REGISTER_CLASS_AND_BASE(Dispatcher1D,Dispatcher DynLibDispatcher);
};


template
<
	class baseClass1, 
	class baseClass2,
	class FunctorType,
	class FunctorReturnType,
	class FunctorArguments,
	bool autoSymmetry=true
>
class Dispatcher2D : public Dispatcher,
				public DynLibDispatcher
				<	  TYPELIST_2(baseClass1,baseClass2)	// base classes for dispatch
					, FunctorType			// class that provides multivirtual call
					, FunctorReturnType			// return type
					, FunctorArguments			// argument of engine unit
					, autoSymmetry
				>
{
	public :
		typedef baseClass1 argType1;
		typedef baseClass2 argType2;
		typedef FunctorType functorType;
		typedef DynLibDispatcher<TYPELIST_2(baseClass1,baseClass2),FunctorType,FunctorReturnType,FunctorArguments,autoSymmetry> dispatcherBase;
		shared_ptr<FunctorType> getFunctor(shared_ptr<baseClass1> arg1, shared_ptr<baseClass2> arg2){ return getExecutor(arg1,arg2); }
		python::dict dump(bool convertIndicesToNames){
			python::dict ret;
			FOREACH(const DynLibDispatcher_Item2D& item, dispatcherBase::dataDispatchMatrix2D()){
				if(convertIndicesToNames){
					string arg1=Dispatcher_indexToClassName<argType1>(item.ix1), arg2=Dispatcher_indexToClassName<argType2>(item.ix2);
					ret[python::make_tuple(arg1,arg2)]=item.functorName;
				} else ret[python::make_tuple(item.ix1,item.ix2)]=item.functorName;
			}
			return ret;
		}
		/* add functor by pointer: this is convenience for calls like foo->add(new SomeFunctor); */
		virtual void add(FunctorType* eu){ add(shared_ptr<FunctorType>(eu)); }
		/* add functor by shared pointer */
		virtual void add(shared_ptr<FunctorType> eu){
			bool dupe=false; string eun=eu->getClassName();
			FOREACH(const shared_ptr<Functor>& f, functors) { if(eun==f->getClassName()) dupe=true; }
			if(!dupe) functors.push_back(eu);
			add2DEntry(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu);
		}

		virtual int getDimension() { return 2; }

		virtual string getFunctorType(){
			shared_ptr<FunctorType> eu(new FunctorType);
			return eu->getClassName();
		}
		virtual string getBaseClassType(unsigned int i){
			if (i==0){ shared_ptr<baseClass1> bc(new baseClass1); return bc->getClassName(); }
			else if (i==1){ shared_ptr<baseClass2> bc(new baseClass2); return bc->getClassName();}
			else return "";
		}
		virtual void Dispatcher_postProcessAttributes_deserializing(){ Dispatcher2D::postProcessAttributes(true); }
		// never gets called directly, but via the proxy above (hack)
		void postProcessAttributes(bool deserializing){
			if(!deserializing) return;
			FOREACH(shared_ptr<Functor> f, functors) add(static_pointer_cast<FunctorType>(f));
		}

	public:
	REGISTER_ATTRIBUTES(Dispatcher,);
	REGISTER_CLASS_AND_BASE(Dispatcher2D,Dispatcher DynLibDispatcher);
};

