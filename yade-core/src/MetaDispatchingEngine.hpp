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

#ifndef __METADISPATCHINGENGINE_HPP__
#define __METADISPATCHINGENGINE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MetaEngine.hpp"
#include "EngineUnit.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

class MetaDispatchingEngine : public MetaEngine
{

	protected : vector<vector<string> > functorNames;
	protected : list<shared_ptr<EngineUnit> > functorArguments;

	public    : typedef list<shared_ptr<EngineUnit> >::iterator EngineUnitListIterator;




	protected : void storeFunctorName(const string& baseClassName1, const string& libName, shared_ptr<EngineUnit> eu);
	protected : void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& libName, shared_ptr<EngineUnit> eu);
	protected : void storeFunctorName(const string& baseClassName1, const string& baseClassName2, const string& baseClassName3, const string& libName, shared_ptr<EngineUnit> eu);
	protected : void storeFunctorArguments(shared_ptr<EngineUnit> eu);
	protected : shared_ptr<EngineUnit> findFunctorArguments(const string& libName);

	public    : MetaDispatchingEngine();
	public    : virtual ~MetaDispatchingEngine();

	public	  : virtual void registerAttributes();
	protected : virtual void postProcessAttributes(bool deserializing);

	public    : virtual string getEngineUnitType() { throw; };
	public    : virtual int getDimension() { throw; };
	public    : virtual string getBaseClassType(unsigned int ) { throw; };

	REGISTER_CLASS_NAME(MetaDispatchingEngine);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(MetaDispatchingEngine,false);


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_ENGINE_UNIT_TYPE(name)		\
	public     : virtual string getEngineUnitType() { return #name; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __METADISPATCHINGENGINE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

