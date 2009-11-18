/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "Engine.hpp"
#include "EngineUnit.hpp"
#include<yade/lib-multimethods/DynLibDispatcher.hpp>

class MetaEngine : public Engine
{
	public:
		vector<vector<string> >		functorNames; // public for python interface; since there is getFunctorArguments returning RW(!) reference to this, why have it private anyway?!
		list<shared_ptr<EngineUnit> >	functorArguments;
	protected:
		void storeFunctorArguments(shared_ptr<EngineUnit> eu);

	public :
		vector<vector<string> >& getFunctorNames();

		typedef list<shared_ptr<EngineUnit> >::iterator EngineUnitListIterator;
		
		//virtual void add( string , string , shared_ptr<EngineUnit> eu = shared_ptr<EngineUnit>()) {throw;}
		//virtual void add( string , string , string , shared_ptr<EngineUnit> eu = shared_ptr<EngineUnit>()) {throw;}
		virtual void add(shared_ptr<EngineUnit> eu=shared_ptr<EngineUnit>()) {throw;}
		virtual void add(EngineUnit*) {throw;}
		virtual void add(string) {throw;}

		virtual list<string> getNeededBex();
		
		void storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<EngineUnit> eu);
		void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<EngineUnit> eu);
		void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<EngineUnit> eu);
		shared_ptr<EngineUnit> findFunctorArguments(const string& libName);
		void clear();
		
		MetaEngine();
		virtual ~MetaEngine();
		
		virtual string getEngineUnitType() { throw; };
		virtual int getDimension() { throw; };
		virtual string getBaseClassType(unsigned int ) { throw; };

	protected:
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Engine,(functorNames)(functorArguments));
	REGISTER_CLASS_AND_BASE(MetaEngine,Engine);
};
REGISTER_SERIALIZABLE(MetaEngine);



template
<
	class baseClass,
	class EngineUnitType,
	class EngineUnitReturnType,
	class EngineUnitArguments,
	bool autoSymmetry=true
>
class MetaEngine1D : public MetaEngine,
				public DynLibDispatcher
				<	  TYPELIST_1(baseClass)		// base classes for dispatch
					, EngineUnitType		// class that provides multivirtual call
					, EngineUnitReturnType		// return type
					, EngineUnitArguments
					, autoSymmetry
				>
{

	public :
		void dump(){ DynLibDispatcher<TYPELIST_1(baseClass),EngineUnitType,EngineUnitReturnType,EngineUnitArguments,autoSymmetry>::dumpDispatchMatrix1D(std::cerr); }
		virtual void add(EngineUnitType* eu){ add(shared_ptr<EngineUnitType>(eu)); }
		virtual void add(shared_ptr<EngineUnitType> eu){
			storeFunctorName(eu->get1DFunctorType1(),eu->getClassName(),eu);
			add1DEntry(eu->get1DFunctorType1(),eu->getClassName(),static_pointer_cast<EngineUnitType>(eu));
		}

		virtual void add(string euType){
			shared_ptr<EngineUnitType> eu=dynamic_pointer_cast<EngineUnitType>(ClassFactory::instance().createShared(euType));
			// if(!eu) throw runtime_error("Class `"+euType+"' could not be cast to required 1D EngineUnit");
			if(!eu){
				cerr<<__FILE__<<":"<<__LINE__<<" WARNING! dynamic cast of engine unit "<<euType<<" failed, will use static_cast. Go figure why."<<endl;
				eu=static_pointer_cast<EngineUnitType>(ClassFactory::instance().createShared(euType));
			}
			add(eu);
		}

		int getDimension() { return 1; }
	
		virtual string getEngineUnitType() 
		{
			shared_ptr<EngineUnitType> eu(new EngineUnitType);
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
			MetaEngine::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add1DEntry(functorNames[i][0],functorNames[i][1],static_pointer_cast<EngineUnitType>(findFunctorArguments(functorNames[i][1])));	
			}
		}
	REGISTER_ATTRIBUTES(MetaEngine,);
	REGISTER_CLASS_AND_BASE(MetaEngine1D,MetaEngine DynLibDispatcher);
};


template
<
	class baseClass1, 
	class baseClass2,
	class EngineUnitType,
	class EngineUnitReturnType,
	class EngineUnitArguments,
	bool autoSymmetry=true
>
class MetaEngine2D : public MetaEngine,
				public DynLibDispatcher
				<	  TYPELIST_2(baseClass1,baseClass2)	// base classes for dispatch
					, EngineUnitType			// class that provides multivirtual call
					, EngineUnitReturnType			// return type
					, EngineUnitArguments			// argument of engine unit
					, autoSymmetry
				>
{
	public :
		void dump(){ DynLibDispatcher<TYPELIST_2(baseClass1,baseClass2),EngineUnitType,EngineUnitReturnType,EngineUnitArguments,autoSymmetry>::dumpDispatchMatrix2D(std::cerr); }
		/* add functor by pointer: this is convenience for calls like foo->add(new SomeFunctor); */
		virtual void add(EngineUnitType* eu){ add(shared_ptr<EngineUnitType>(eu)); }
		/* add functor by shared pointer */
		virtual void add(shared_ptr<EngineUnitType> eu){
			storeFunctorName(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),eu);
			add2DEntry(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),static_pointer_cast<EngineUnitType>(eu));
		}
		/* add functor by its literal name */
		virtual void add(string euType){
			shared_ptr<EngineUnitType> eu=static_pointer_cast<EngineUnitType>(ClassFactory::instance().createShared(euType));
			add(eu);
		}


		virtual int getDimension() { return 2; }

		virtual string getEngineUnitType() 
		{
			shared_ptr<EngineUnitType> eu(new EngineUnitType);
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
			MetaEngine::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add2DEntry(functorNames[i][0],functorNames[i][1],functorNames[i][2],static_pointer_cast<EngineUnitType>(findFunctorArguments(functorNames[i][2])));
			}
		}
	REGISTER_ATTRIBUTES(MetaEngine,);
	REGISTER_CLASS_AND_BASE(MetaEngine2D,MetaEngine DynLibDispatcher);
};

