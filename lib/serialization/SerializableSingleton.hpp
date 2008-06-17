/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SERIALIZABLESINGLETON_HPP
#define SERIALIZABLESINGLETON_HPP

#include "SerializableTypes.hpp"
#include <typeinfo>
#include <string>
#include <map>
#include<yade/lib-loki/Singleton.hpp>
#include<yade/lib-base/Logging.hpp>

using namespace std;

class SerializableSingleton : public Singleton< SerializableSingleton >
{
	private :
		/// Pointer on a function that return the type_info of the registered class
		typedef const type_info& ( *VerifyFactorableFnPtr )();
		/// Description of a class that is stored inside the factory.
		struct SerializableDescriptor
		{
			/// Used by the findType method to test the type of the class and know if it is a Factorable (i.e. Factorable) or Custom class
			VerifyFactorableFnPtr verify;
			SerializableTypes::Type type;
			bool fundamental;

			SerializableDescriptor() {};
			SerializableDescriptor(	VerifyFactorableFnPtr v, SerializableTypes::Type t, bool f)
			{
				verify 		 = v;
				type   		 = t;
				fundamental 	 = f;
			};
		};

	/// Type of a Stl map used to map the registered class name with their SerializableDescriptor
		typedef std::map< std::string , SerializableDescriptor > SerializableDescriptorMap;
		SerializableDescriptorMap myMap;

	public :
		bool registerSerializableDescriptor( string name, VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f);
		bool findClassInfo(const type_info& tp,SerializableTypes::Type& type, string& serializableClassName,bool& fundamental);
		DECLARE_LOGGER;

	private :
		SerializableSingleton();
		SerializableSingleton(const SerializableSingleton&);
		~SerializableSingleton() {};
		SerializableSingleton& operator=(const SerializableSingleton&);

	FRIEND_SINGLETON(SerializableSingleton);
};

#endif // SERIALIZABLESINGLETON_HPP

