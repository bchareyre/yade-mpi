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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SERIALIZABLESINGLETON_HPP__
#define __SERIALIZABLESINGLETON_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializableTypes.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <typeinfo>
#include <string>
#include <map>

#include <yade-lib-loki/Singleton.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SerializableSingleton : public Singleton< SerializableSingleton >
{

	/*! Pointer on a function that return the type_info of the registered class */
	private   : typedef const type_info& ( *VerifyFactorableFnPtr )();

	/*! Description of a class that is stored inside the factory.*/
	private   : class SerializableDescriptor
		    {
			///////////////////////////////////////////////////////////////////////////
			/// Attributes								///
			///////////////////////////////////////////////////////////////////////////

			/*! Used by the findType method to test the type of the class and know if it is a Factorable (i.e. Factorable) or Custom class*/
			public    : VerifyFactorableFnPtr verify;
			/*! Type of the class : SERIALIZABLE,CUSTOM,CONTAINER,POINTER */
			public    : SerializableTypes::Type type;
			/*! fundamental is true the class type is a fundamtental type (e.g. Vector3, Quaternion) */
			public    : bool fundamental;

			///////////////////////////////////////////////////////////////////////////
			/// Constructor/Destructor						///
			///////////////////////////////////////////////////////////////////////////

			/*! Empty constructor */
			public    : SerializableDescriptor() {};
			/*! Constructor that initialize all the attributes of the class */
			public    : SerializableDescriptor(	VerifyFactorableFnPtr v, SerializableTypes::Type t, bool f)
				    {
					verify 		 = v;
					type   		 = t;
					fundamental 	 = f;
				    };

		    };

	/*! Type of a Stl map used to map the registered class name with their SerializableDescriptor */
	private   : typedef std::map< std::string , SerializableDescriptor > SerializableDescriptorMap;
	private   : SerializableDescriptorMap map;
	public    : bool registerSerializableDescriptor( string name, VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f);
	public    : bool findClassInfo(const type_info& tp,SerializableTypes::Type& type, string& serializableClassName,bool& fundamental);

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor
		\note  the constructor is private because ClassFactory is a Singleton
	*/
	private   : SerializableSingleton();

	/*! Copy Constructor
		\note  needed by the singleton class
	*/
	private   : SerializableSingleton(const SerializableSingleton&);

	/*! Destructor
		\note  the destructor is private because ClassFactory is a Singleton
	*/
	private   : ~SerializableSingleton() {};

	/*! Assignement operator needed by the Singleton class */
	private   : SerializableSingleton& operator=(const SerializableSingleton&);

	FRIEND_SINGLETON(SerializableSingleton);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SERIALIZABLESINGLETON_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
