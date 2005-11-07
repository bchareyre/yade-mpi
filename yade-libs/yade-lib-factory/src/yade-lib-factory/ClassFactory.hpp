/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CLASSFACTORY_HPP
#define CLASSFACTORY_HPP


#include <map>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>


#include <yade/yade-lib-loki/Singleton.hpp>


#include "FactoryExceptions.hpp"
#include "DynLibManager.hpp"


#define REGISTER_FACTORABLE(name) 						\
	inline boost::shared_ptr< Factorable > CreateShared##name()			\
	{										\
		return boost::shared_ptr< name > ( new name );				\
	}										\
	inline Factorable* Create##name()						\
	{										\
		return new name;							\
	}										\
	inline void * CreatePureCustom##name()						\
	{										\
		return new name;							\
	}										\
	const bool registered##name =							\
		ClassFactory::instance().registerFactorable( 	#name ,			\
								Create##name ,		\
								CreateShared##name ,	\
								CreatePureCustom##name);


class Factorable;


/*! \brief The class factory of Yade used for serialization purpose and also as a dynamic library loader.
	All classes that call the macro REGISTER_FACTORABLE in their header are registered inside the factory
	so it is possible to ask the factory to create an instance of that class. This is automatic because the
	macro should be outside the class definition, so it is called automatically when the class is loaded by
	the program or when a dynamic library is loaded.
	
	This ClassFactory also acts as a dynamic library loader : when you ask for an instance, either the class
	already exists inside the factory and a new instance is created, or the class doesn't exist inside the
	factory and the ClassFactory will look on the hard drive to know if your class exists inside a dynamic library.
	If so the library is loaded and a new instance of the class can be created.

	\note ClassFactory is a singleton so you can't create an instance of it because its constructor is private.
	You should instead use ClassFactory::instance().createShared("Rigidbody") for example.
*/
class ClassFactory : public Singleton< ClassFactory >
{

/// Types

	/*! Pointer on a function that create an instance of a serializable class an return a shared pointer on it */
	private   : typedef boost::shared_ptr<Factorable> ( *CreateSharedFactorableFnPtr )();
	/*! Pointer on a function that create an instance of a serializable class an return a C pointer on it */
	private   : typedef Factorable* ( *CreateFactorableFnPtr )();
	/*! Pointer on a function that create an instance of a custom class (i.e. not serializable) and return a void C pointer on it */
	private   : typedef void* ( *CreatePureCustomFnPtr )();
	
	/*! Description of a class that is stored inside the factory.*/
	private   : class FactorableCreators
		    {
			/// Attributes

			/*! Used to create a C pointer on the class (if serializable) */
			public    : CreateFactorableFnPtr create;
			/*! Used to create a shared pointer on the class (if serializable) */
			public    : CreateSharedFactorableFnPtr createShared;
			/*! Used to create a void C pointer on the class */
			public    : CreatePureCustomFnPtr createPureCustom;
	
			/// Constructor/Destructor

			/*! Empty constructor */
			public    : FactorableCreators() {};
			/*! Constructor that initialize all the attributes of the class */
			public    : FactorableCreators(	CreateFactorableFnPtr c, CreateSharedFactorableFnPtr cs,
							CreatePureCustomFnPtr cpc)
				    {
					create 		 = c;
					createShared	 = cs;
					createPureCustom = cpc;
				    };

		    };

 	/*! Type of a Stl map used to map the registered class name with their FactorableCreators */
	private   : typedef std::map< std::string , FactorableCreators > FactorableCreatorsMap;


/// Attributes

	/*! The internal dynamic library manager used to load dynamic libraries when an instance of a non loaded class is ask */
	private   : DynLibManager dlm;
	/*! Map that contains the name of the registered class and their description */
	private   : FactorableCreatorsMap map;

/// Constructor/Destructor

	/*! Constructor
		\note  the constructor is private because ClassFactory is a Singleton
	*/
	private   : ClassFactory() {	cerr << "Constructing ClassFactory  (if multiple times - check '-rdynamic' flag!)" << endl;};
	/*! Copy Constructor
		\note  needed by the singleton class
	*/
	private   : ClassFactory(const ClassFactory&);
	
	/*! Destructor
		\note  the destructor is private because ClassFactory is a Singleton
	*/
	private   : virtual ~ClassFactory() {};

/// Methods

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
						CreateSharedFactorableFnPtr createShared, CreatePureCustomFnPtr createPureCustom);

	/*! Create a shared pointer on a serializable class of the given name */
	public 	  : boost::shared_ptr<Factorable> createShared( std::string name );

	/*! Create a C pointer on a serializable class of the given name */
	public 	  : Factorable* createPure( std::string name );

	/*! Create a void C pointer on a class of the given name */
	public 	  : void * createPureCustom( std::string name );

	/*! Mainly used by the method findType for serialization purpose. Tells if a given type is a serilializable class
		\param tp type info of the type to test
		\param fundamental is true if the given type is fundamental (Vector3,Quaternion ...)
	*/
	public 	  : bool isFactorable(const type_info& tp,bool& fundamental);

	public 	  : void addBaseDirectory(const string& dir);
	
	public    : bool load(const string& name );

	public    : string libNameToSystemName(const string& name);
	public	  : string systemNameToLibName(const string& name);

	public    : virtual string getClassName() const { return "Factorable"; };
	public    : virtual string getBaseClassName(int ) const { return "";};

	FRIEND_SINGLETON(ClassFactory);
};

#endif // __CLASSFACTORY_HPP__

