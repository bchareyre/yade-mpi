/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef __CLASSFACTORY_HPP__
#define __CLASSFACTORY_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FactoryExceptions.hpp"
#include "Singleton.hpp"
#include "ArchiveTypes.hpp"
#include "DynLibManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ArchiveTypes;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_CLASS_TO_FACTORY(name,sname,type,isFundamental) 			\
	inline boost::shared_ptr< Factorable > CreateShared##sname()			\
	{										\
		return boost::shared_ptr< sname > ( new sname );			\
	}										\
	inline Factorable* Create##sname()						\
	{										\
		return new sname;							\
	}										\
	inline void * CreatePureCustom##name()						\
	{										\
		return new name;							\
	}										\
	inline const type_info& Verify##name()						\
	{										\
		return typeid(name);							\
	}										\
	const bool registered##name##sname =						\
		ClassFactory::instance().registerFactorable( 	#sname ,		\
								Create##sname ,		\
								CreateShared##sname ,	\
								CreatePureCustom##name,	\
								Verify##name ,		\
								type,			\
								isFundamental );

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Factorable;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The class factory of Yade used for serialization purpose and also as a dynamic library loader.
	All classes that call the macro REGISTER_CLASS in their header are registered inside the factory so it is possible to ask the factory to create an instance of that class. This is automatic because the macro should be outside the class definition, so it is called automatically when the class is loaded by the program or when a dynamic library is loaded. This ClassFactory also acts as a dynamic library loader : when you ask for an instance, either the class already exists inside the factory and a new instance is created, either the class doesn't exist inside the factory and the ClassFactory will look on the hard drive to know if your class exists inside a dynamic library. If so the library is loaded and a new instance of the class can be created.
	\note ClassFactory is a singleton so you can't create an instance of it because its constructor is private. You should instead use ClassFactory::instance().createShared("Rigidbody") for example
*/
class ClassFactory : public Singleton< ClassFactory >
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Types											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Pointer on a function that create an instance of a serializable class an return a shared pointer on it */
	private   : typedef boost::shared_ptr<Factorable> ( *CreateSharedFactorableFnPtr )();
	/*! Pointer on a function that create an instance of a serializable class an return a C pointer on it */
	private   : typedef Factorable* ( *CreateFactorableFnPtr )();
	/*! Pointer on a function that create an instance of a custom class (i.e. not serializable) and return a void C pointer on it */
	private   : typedef void* ( *CreatePureCustomFnPtr )();
	/*! Pointer on a function that return the type_info of the registered class */
	private   : typedef const type_info& ( *VerifyFactorableFnPtr )();

	/*! Description of a class that is stored inside the factory.*/
	private   : class ClassDescriptor
		    {
			///////////////////////////////////////////////////////////////////////////
			/// Attributes								///
			///////////////////////////////////////////////////////////////////////////

			/*! Used to create a C pointer on the class (if serializable) */
			public    : CreateFactorableFnPtr create;
			/*! Used to create a shared pointer on the class (if serializable) */
			public    : CreateSharedFactorableFnPtr createShared;
			/*! Used to create a void C pointer on the class */
			public    : CreatePureCustomFnPtr createPureCustom;
			/*! Used by the findType method to test the type of the class and know if it is a Factorable (i.e. Factorable) or Custom class*/
			public    : VerifyFactorableFnPtr verify;
			/*! Type of the class : SERIALIZABLE,CUSTOM,CONTAINER,POINTER */
			public    : RecordType type;
			/*! fundamental is true the class type is a fundamtental type (e.g. Vector3, Quaternion) */
			public    : bool fundamental;

			///////////////////////////////////////////////////////////////////////////
			/// Constructor/Destructor						///
			///////////////////////////////////////////////////////////////////////////

			/*! Empty constructor */
			public    : ClassDescriptor() {};
			/*! Constructor that initialize all the attributes of the class */
			public    : ClassDescriptor(	CreateFactorableFnPtr c, CreateSharedFactorableFnPtr cs,
							CreatePureCustomFnPtr cpc, VerifyFactorableFnPtr v,
							RecordType t, bool f)
				    {
					create 		 = c;
					createShared	 = cs;
					createPureCustom = cpc;
					verify 		 = v;
					type   		 = t;
					fundamental 	 = f;
				    };

		    };

 	/*! Type of a Stl map used to map the registered class name with their ClassDescription */
	private   : typedef std::map< std::string , ClassDescriptor > ClassDescriptorMap;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! The internal dynamic library manager used to load dynamic libraries when an instance of a non loaded class is ask */
	private   : DynLibManager dlm;
	/*! Map that contains the name of the registered class and their description */
	private   : ClassDescriptorMap map;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor
		\note  the constructor is private because ClassFactory is a Singleton
	*/
	private   : ClassFactory() {};
	/*! Copy Constructor
		\note  needed by the singleton class
	*/
	private   : ClassFactory(const ClassFactory&);
	/*! Destructor
		\note  the destructor is private because ClassFactory is a Singleton
	*/
	private   : ~ClassFactory() {};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Assignement operator needed by the Singleton class */
	private   : ClassFactory& operator=(const ClassFactory&);

	/*! This method is used to register a Factorable class into the factory. It is called only from macro REGISTER_CLASS_TO_FACTORY
		\param name the name of the class
		\param create a pointer to a function that is able to return a C pointer on the given class
		\param createPureCustom a pointer to a function that is able to return a void C pointer on the given class
		\param verify a pointer to a function that is able to return the type_info of the given class
		\param type type of the class (SERIALIZABLE or CUSTOM)
		\param f is true is the class is a fundamental one (Vector3, Quaternion)
		\return true if registration is succesfull
	*/
	public    : bool registerFactorable( 	std::string name			  , CreateFactorableFnPtr create,
						CreateSharedFactorableFnPtr createShared, CreatePureCustomFnPtr createPureCustom,
						VerifyFactorableFnPtr verify		  , RecordType type, bool f );

	/*! Create a shared pointer on a serializable class of the given name */
	public 	  : boost::shared_ptr<Factorable> createShared( std::string name );

	/*! Create a C pointer on a serializable class of the given name */
	public 	  : Factorable* createPure( std::string name );

	/*! Create a void C pointer on a class of the given name */
	public 	  : void * createPureCustom( std::string name );

	/*! Mainly used by the method findType for serialization purpose. Tells if a given type is a custom class
		\param tp type info of the type to test
		\param serializableClassName name of the serializable version of the class corresponding to "tp"
		\param fundamental is true if the given type is fundamental (Vector3,Quaternion ...)
	*/
	public 	  : bool isCustomClass(const type_info& tp,string& serializableClassName,bool& fundamental);

	/*! Mainly used by the method findType for serialization purpose. Tells if a given type is a serilializable class
		\param tp type info of the type to test
		\param fundamental is true if the given type is fundamental (Vector3,Quaternion ...)
	*/
	public 	  : bool isFactorable(const type_info& tp,bool& fundamental);

	public    : bool findClassInfo(const type_info& tp,RecordType& type, string& serializableClassName,bool& fundamental);


	friend class Singleton< ClassFactory >;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __CLASSFACTORY_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
