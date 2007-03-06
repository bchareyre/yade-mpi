/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METADISPATCHINGENGINE1D_HPP
#define METADISPATCHINGENGINE1D_HPP

#include "MetaDispatchingEngine.hpp"
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>

/*! Type-safe variant of the MetaDispatchingEngine2D::add function. Arguments are not strings, but regular typenames and will give compile-time error if not defined. They are passed to MetaDispatchingEngine1D::add in the same order as given. */
#define DISPATCHER_ADD2(e1,e2) add(#e1,#e2); {/* compile-time check for class existence */ typedef e1 p1; typedef e2 p2;}
/*! Same as DISPATCHER_ADD2 macro, but passes the additional 3rd argument to MetaDispatchingEngine1D::add as its 3rd, optional argument */
#define DISPATCHER_ADD2_1(e1,e2,e3) add(#e1,#e2,e3); {/* compile-time check for class existence */ typedef e1 p1; typedef e2 p2;}


template
<
	class baseClass,
	class EngineUnitType,
	class EngineUnitReturnType,
	class EngineUnitArguments,
	bool autoSymmetry=true
>
class MetaDispatchingEngine1D : public MetaDispatchingEngine,
				public DynLibDispatcher
				<	  TYPELIST_1(baseClass)		// base classes for dispatch
					, EngineUnitType		// class that provides multivirtual call
					, EngineUnitReturnType		// return type
					, EngineUnitArguments
					, autoSymmetry
				>
{

	public :
		virtual void add( string baseClassName1, string libName, shared_ptr<EngineUnit> eu = shared_ptr<EngineUnitType>())
		{
			storeFunctorName(baseClassName1,libName,static_pointer_cast<EngineUnitType>(eu));
			add1DEntry(baseClassName1,libName,static_pointer_cast<EngineUnitType>(eu));
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
		void registerAttributes()
		{
			MetaDispatchingEngine::registerAttributes();
		}
		void postProcessAttributes(bool deserializing)
		{
			MetaDispatchingEngine::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add1DEntry(functorNames[i][0],functorNames[i][1],static_pointer_cast<EngineUnitType>(findFunctorArguments(functorNames[i][1])));	
			}
		}
	REGISTER_CLASS_NAME(MetaDispatchingEngine1D);
	REGISTER_BASE_CLASS_NAME(MetaDispatchingEngine DynLibDispatcher);
};

#endif // METADISPATCHINGENGINE1D_HPP

