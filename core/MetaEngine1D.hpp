/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METADISPATCHINGENGINE1D_HPP
#define METADISPATCHINGENGINE1D_HPP

#include "MetaEngine.hpp"
#include<yade/lib-multimethods/DynLibDispatcher.hpp>

/*! Type-safe variant of the MetaEngine2D::add function. Arguments are not strings, but regular typenames and will give compile-time error if not defined. They are passed to MetaEngine1D::add in the same order as given. */
#define DISPATCHER_ADD2(e1,e2) add(#e1,#e2); {/* compile-time check for class existence */ typedef e1 p1; typedef e2 p2;}
/*! Same as DISPATCHER_ADD2 macro, but passes the additional 3rd argument to MetaEngine1D::add as its 3rd, optional argument */
#define DISPATCHER_ADD2_1(e1,e2,e3) add(#e1,#e2,e3); {/* compile-time check for class existence */ typedef e1 p1; typedef e2 p2;}


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
		#if 0
		virtual void __attribute__((deprecated)) add(string baseClassName1, string libName, shared_ptr<EngineUnit> eu = shared_ptr<EngineUnitType>()) {
			storeFunctorName(baseClassName1,libName,static_pointer_cast<EngineUnitType>(eu));
			add1DEntry(baseClassName1,libName,static_pointer_cast<EngineUnitType>(eu));
		}
		#endif

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

#endif // METADISPATCHINGENGINE1D_HPP

