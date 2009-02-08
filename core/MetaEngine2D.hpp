/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "MetaEngine.hpp"
#include<yade/lib-multimethods/DynLibDispatcher.hpp>


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
		/* add functor by pointer: this is convenience for calls like foo->add(new SomeFunctor); */
		virtual void add(EngineUnitType* eu){ add(shared_ptr<EngineUnitType>(eu)); }
		/* add functor by shared pointer */
		virtual void add(shared_ptr<EngineUnitType> eu){
			storeFunctorName(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),eu);
			add2DEntry(eu->get2DFunctorType1(),eu->get2DFunctorType2(),eu->getClassName(),static_pointer_cast<EngineUnitType>(eu));
		}
		/* add functor by its literal name */
		virtual void add(string euType){
			shared_ptr<EngineUnitType> eu=dynamic_pointer_cast<EngineUnitType>(ClassFactory::instance().createShared(euType));
			if(!eu){
				cerr<<__FILE__<<":"<<__LINE__<<" WARNING! dynamic cast of engine unit "<<euType<<" failed, will use static_cast. Go figure why."<<endl;
				eu=static_pointer_cast<EngineUnitType>(ClassFactory::instance().createShared(euType));
			}
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

