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
#include<yade/lib-multimethods/DynLibDispatcher.hpp>

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
		void dump(){ DynLibDispatcher<TYPELIST_1(baseClass),FunctorType,FunctorReturnType,FunctorArguments,autoSymmetry>::dumpDispatchMatrix1D(std::cerr); }
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
		void dump(){ DynLibDispatcher<TYPELIST_2(baseClass1,baseClass2),FunctorType,FunctorReturnType,FunctorArguments,autoSymmetry>::dumpDispatchMatrix2D(std::cerr); }
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

