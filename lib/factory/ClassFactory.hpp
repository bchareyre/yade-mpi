/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<map>
#include<string>
#include<list>
#include<iostream>

#ifndef  __GXX_EXPERIMENTAL_CXX0X__
#	include<boost/shared_ptr.hpp>
	using boost::shared_ptr;
#else
#	include<memory>
	using std::shared_ptr;
#endif


#include<yade/lib-loki/Singleton.hpp>


#include "FactoryExceptions.hpp"
#include "DynLibManager.hpp"


#define REGISTER_FACTORABLE(name) 						\
	inline shared_ptr< Factorable > CreateShared##name()			\
	{										\
		return shared_ptr< name > ( new name );				\
	}										\
	inline Factorable* Create##name()						\
	{										\
		return new name;							\
	}										\
	inline void * CreatePureCustom##name()						\
	{										\
		return new name;							\
	}										\
	const bool registered##name __attribute__ ((unused)) =							\
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
	private :
		/// Pointer on a function that create an instance of a serializable class an return a shared pointer on it
		typedef shared_ptr<Factorable> ( *CreateSharedFactorableFnPtr )();
		/// Pointer on a function that create an instance of a serializable class an return a C pointer on it
		typedef Factorable* ( *CreateFactorableFnPtr )();
		/// Pointer on a function that create an instance of a custom class (i.e. not serializable) and return a void C pointer on it
		typedef void* ( *CreatePureCustomFnPtr )();
	
		/// Description of a class that is stored inside the factory.
		struct FactorableCreators
		{
			CreateFactorableFnPtr create;		/// Used to create a C pointer on the class (if serializable)
			CreateSharedFactorableFnPtr createShared;	/// Used to create a shared pointer on the class (if serializable)
			CreatePureCustomFnPtr createPureCustom;	/// Used to create a void C pointer on the class
	
			FactorableCreators() {};
			FactorableCreators(	CreateFactorableFnPtr c, CreateSharedFactorableFnPtr cs,
						CreatePureCustomFnPtr cpc)
			{
				create 		 = c;
				createShared	 = cs;
				createPureCustom = cpc;
			};
		};

	 	/// Type of a Stl map used to map the registered class name with their FactorableCreators
		typedef std::map< std::string , FactorableCreators > FactorableCreatorsMap;

		/// The internal dynamic library manager used to load dynamic libraries when an instance of a non loaded class is ask
		DynLibManager dlm;
		/// Map that contains the name of the registered class and their description
		FactorableCreatorsMap map;

		ClassFactory() { if(getenv("YADE_DEBUG")) cerr<<"Constructing ClassFactory; _must_ be only once, otherwise linking is broken (missing -rdynamic?)\n"<<endl; };
		ClassFactory(const ClassFactory&);
		ClassFactory& operator=(const ClassFactory&);
		virtual ~ClassFactory() {};

	public :
		/*! This method is used to register a Factorable class into the factory. It is called only from macro REGISTER_CLASS_TO_FACTORY
			\param name the name of the class
			\param create a pointer to a function that is able to return a C pointer on the given class
			\param createPureCustom a pointer to a function that is able to return a void C pointer on the given class
			\param verify a pointer to a function that is able to return the type_info of the given class
			\param type type of the class (SERIALIZABLE or CUSTOM)
			\param f is true is the class is a fundamental one (Vector3, Quaternion)
			\return true if registration is succesfull
		*/
		bool registerFactorable( 	std::string name			  , CreateFactorableFnPtr create,
						CreateSharedFactorableFnPtr createShared, CreatePureCustomFnPtr createPureCustom);

		/// Create a shared pointer on a serializable class of the given name
		shared_ptr<Factorable> createShared( std::string name );

		/// Create a C pointer on a serializable class of the given name
		Factorable* createPure( std::string name );

		/// Create a void C pointer on a class of the given name
		void * createPureCustom( std::string name );

		/*! Mainly used by the method findType for serialization purpose. Tells if a given type is a serilializable class
			\param tp type info of the type to test
			\param fundamental is true if the given type is fundamental (Vector3,Quaternion ...)
		*/
		bool isFactorable(const type_info& tp,bool& fundamental);

		void addBaseDirectory(const string& dir);

		bool load(const string& name);
		std::string lastError();

		void registerPluginClasses(const char* fileAndClasses[]);
		list<string> pluginClasses;

		string libNameToSystemName(const string& name);
		string systemNameToLibName(const string& name);

		virtual string getClassName() const { return "Factorable"; };
		virtual string getBaseClassName(int ) const { return "";};

	FRIEND_SINGLETON(ClassFactory);
};


/*! Macro defining what classes can be found in this plugin -- must always be used in the respective .cpp file. If left empty, filename will be used to deduce that.
 *
 * Note:
 * 	1. Visibility must be set to "internal" (or "protected") so that other plugins' init will not shadow this one and all of them get properly executed.
 * 	2. The function must be enclosed in its own anonymous namespace, otherwise there will be clashes (liker errors) if more files with YADE_PLUGIN are linked together.
 */
#define YADE_PLUGIN(...) namespace{ __attribute__((constructor)) __attribute__((visibility("internal"))) void registerThisPluginClasses(void){ const char* info[]={__FILE__ , ##__VA_ARGS__ , NULL}; ClassFactory::instance().registerPluginClasses(info);} }

