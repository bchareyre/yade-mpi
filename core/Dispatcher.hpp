/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "Engine.hpp"
#include "Functor.hpp"
#include<yade/core/Omega.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<boost/lexical_cast.hpp>

class Dispatcher : public Engine
{
	public:
		vector<vector<string> >		functorNames; // public for python interface; since there is getFunctorArguments returning RW(!) reference to this, why have it private anyway?!
		list<shared_ptr<Functor> >	functorArguments;
	protected:
		void storeFunctorArguments(shared_ptr<Functor> eu);

	public :
		vector<vector<string> >& getFunctorNames();

		typedef list<shared_ptr<Functor> >::iterator FunctorListIterator;
		
		//virtual void add( string , string , shared_ptr<Functor> eu = shared_ptr<Functor>()) {throw;}
		//virtual void add( string , string , string , shared_ptr<Functor> eu = shared_ptr<Functor>()) {throw;}
		virtual void add(shared_ptr<Functor> eu=shared_ptr<Functor>()) {throw;}
		virtual void add(Functor*) {throw;}
		virtual void add(string) {throw;}

		//! Should be called by dispatcher before every loop (could be somehow optimized, since it will change very rarely)
		void updateScenePtr() { FOREACH(shared_ptr<Functor> f, functorArguments){ f->scene=scene; } }

		void storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<Functor> eu);
		void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<Functor> eu);
		void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<Functor> eu);
		shared_ptr<Functor> findFunctorArguments(const string& libName);
		void clear();
		
		Dispatcher();
		virtual ~Dispatcher();
		
		virtual string getFunctorType() { throw; };
		virtual int getDimension() { throw; };
		virtual string getBaseClassType(unsigned int ) { throw; };


	protected:
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Engine,(functorNames)(functorArguments));
	REGISTER_CLASS_AND_BASE(Dispatcher,Engine);
};
REGISTER_SERIALIZABLE(Dispatcher);

/*! Function returning class name (as string) for given index and topIndexable (top-level indexable, such as Shape, Material and so on)
This function exists solely for debugging, is quite slow: it has to traverse all classes and ask for inheritance information.
It should be used primarily to convert indices to names in Dispatcher::dictDispatchMatrix?D; since it relies on Omega for RTTI,
this code could not be in Dispatcher itself.
*/
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

		shared_ptr<FunctorType> getFunctor(shared_ptr<baseClass>& arg){ return getExecutor(arg); }
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
			storeFunctorName(eu->get1DFunctorType1(),eu->getClassName(),eu);
			add1DEntry(eu->get1DFunctorType1(),eu->getClassName(),static_pointer_cast<FunctorType>(eu));
		}

		virtual void add(string euType){
			shared_ptr<FunctorType> eu=dynamic_pointer_cast<FunctorType>(ClassFactory::instance().createShared(euType));
			// if(!eu) throw runtime_error("Class `"+euType+"' could not be cast to required 1D Functor");
			if(!eu){
				cerr<<__FILE__<<":"<<__LINE__<<" WARNING! dynamic cast of engine unit "<<euType<<" failed, will use static_cast. Go figure why."<<endl;
				eu=static_pointer_cast<FunctorType>(ClassFactory::instance().createShared(euType));
			}
			add(eu);
		}

		int getDimension() { return 1; }
	
		virtual string getFunctorType() 
		{
			shared_ptr<FunctorType> eu(new FunctorType);
			return eu->getClassName();
		}

		virtual string getBaseClassType(unsigned int i)
		{
			if (i==0)
			{
				shared_ptr<baseClass> bc(new baseClass);
				return bc->getClassName();
			}
			else
				return "";
		}

	protected :
		void postProcessAttributes(bool deserializing)
		{
			Dispatcher::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add1DEntry(functorNames[i][0],functorNames[i][1],static_pointer_cast<FunctorType>(findFunctorArguments(functorNames[i][1])));	
			}
		}
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
		shared_ptr<FunctorType> getFunctor(shared_ptr<baseClass1>& arg1, shared_ptr<baseClass2>& arg2){ return getExecutor(arg1,arg2); }
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
			storeFunctorName(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),eu);
			add2DEntry(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),static_pointer_cast<FunctorType>(eu));
		}
		/* add functor by its literal name */
		virtual void add(string euType){
			shared_ptr<FunctorType> eu=static_pointer_cast<FunctorType>(ClassFactory::instance().createShared(euType));
			add(eu);
		}


		virtual int getDimension() { return 2; }

		virtual string getFunctorType() 
		{
			shared_ptr<FunctorType> eu(new FunctorType);
			return eu->getClassName();
		}
	
		virtual string getBaseClassType(unsigned int i)
		{
			if (i==0)
			{
				shared_ptr<baseClass1> bc(new baseClass1);
				return bc->getClassName();
			}
			else if (i==1)
			{
				shared_ptr<baseClass2> bc(new baseClass2);
				return bc->getClassName();
			}
			else
				return "";
		}

	protected :
		void postProcessAttributes(bool deserializing)
		{
			Dispatcher::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add2DEntry(functorNames[i][0],functorNames[i][1],functorNames[i][2],static_pointer_cast<FunctorType>(findFunctorArguments(functorNames[i][2])));
			}
		}
	REGISTER_ATTRIBUTES(Dispatcher,);
	REGISTER_CLASS_AND_BASE(Dispatcher2D,Dispatcher DynLibDispatcher);
};

