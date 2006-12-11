/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METADISPATCHINGENGINE2D_HPP
#define METADISPATCHINGENGINE2D_HPP

#include "MetaDispatchingEngine.hpp"
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>

template
<
	class baseClass1, 
	class baseClass2,
	class EngineUnitType,
	class EngineUnitReturnType,
	class EngineUnitArguments,
	bool autoSymmetry=true
>
class MetaDispatchingEngine2D : public MetaDispatchingEngine,
				public DynLibDispatcher
				<	  TYPELIST_2(baseClass1,baseClass2)	// base classes for dispatch
					, EngineUnitType			// class that provides multivirtual call
					, EngineUnitReturnType			// return type
					, EngineUnitArguments			// argument of engine unit
					, autoSymmetry
				>
{
	public :
		virtual void add( string baseClassName1, string baseClassName2, string libName, shared_ptr<EngineUnit> eu = shared_ptr<EngineUnitType>())
		{
			storeFunctorName(baseClassName1,baseClassName2,libName,static_pointer_cast<EngineUnitType>(eu));
			add2DEntry(baseClassName1,baseClassName2,libName,static_pointer_cast<EngineUnitType>(eu));
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
			MetaDispatchingEngine::postProcessAttributes(deserializing);
			if(deserializing)
			{
				for(unsigned int i=0;i<functorNames.size();i++)
					add2DEntry(functorNames[i][0],functorNames[i][1],functorNames[i][2],static_pointer_cast<EngineUnitType>(findFunctorArguments(functorNames[i][2])));
			}
		}

		void registerAttributes()
		{
			MetaDispatchingEngine::registerAttributes();
		}
	REGISTER_CLASS_NAME(MetaDispatchingEngine2D);
	REGISTER_BASE_CLASS_NAME(MetaDispatchingEngine DynLibDispatcher);

};

//REGISTER_SERIALIZABLE(MetaDispatchingEngine2D,false);

/*
#define REGISTER_BASE_CLASS_TYPE_2D(name1,name2)			\
 	public : virtual string getBaseClassType(unsigned int i)	\
 	{								\
 		switch (i)						\
 		{							\
 			case 0  : return #name1;			\
 			case 1  : return #name2;			\
 			default : return "";				\
 		}							\
 	}	
*/

#endif // METADISPATCHINGENGINE2D_HPP

