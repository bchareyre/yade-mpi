/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __METADISPATCHINGENGINE2D_HPP__
#define __METADISPATCHINGENGINE2D_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MetaDispatchingEngine.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

	public : void add( string baseClassName1, string baseClassName2, string libName, shared_ptr<EngineUnitType> eu = shared_ptr<EngineUnitType>())
	{
		storeFunctorName(baseClassName1,baseClassName2,libName,eu);
		add2DEntry(baseClassName1,baseClassName2,libName,eu);
	}

	protected : void postProcessAttributes(bool deserializing)
	{
		MetaDispatchingEngine::postProcessAttributes(deserializing);
	
		if(deserializing)
		{
			for(unsigned int i=0;i<functorNames.size();i++)
				add2DEntry(functorNames[i][0],functorNames[i][1],functorNames[i][2],static_pointer_cast<EngineUnitType>(findFunctorArguments(functorNames[i][2])));
		}
	}

	public : void registerAttributes()
	{
		MetaDispatchingEngine::registerAttributes();
	}

	public    : virtual int getDimension() { return 2; }

	public    : virtual string getEngineUnitType() 
	{
		shared_ptr<EngineUnitType> eu(new EngineUnitType);
		return eu->getClassName();
	}

	public    : virtual string getBaseClassType(unsigned int i)
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


	REGISTER_CLASS_NAME(MetaDispatchingEngine2D);
	REGISTER_BASE_CLASS_NAME(MetaDispatchingEngine DynLibDispatcher);

};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//REGISTER_SERIALIZABLE(MetaDispatchingEngine2D,false);

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

// #define REGISTER_BASE_CLASS_TYPE_2D(name1,name2)			\
// 	public : virtual string getBaseClassType(unsigned int i)	\
// 	{								\
// 		switch (i)						\
// 		{							\
// 			case 0  : return #name1;			\
// 			case 1  : return #name2;			\
// 			default : return "";				\
// 		}							\
// 	}	

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __METADISPATCHINGENGINE2D_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

