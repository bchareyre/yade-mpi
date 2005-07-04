/***************************************************************************
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

// it's magic :)

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MULTITYPEHANDLER_H__
#define __MULTITYPEHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializationExceptions.hpp"
#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <utility>

#include <boost/tuple/tuple.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Type>
struct MultiTypeSerializer : public Serializable
{
	public : MultiTypeSerializer() {};
	public : virtual ~MultiTypeSerializer() {};
};


template<typename Type>
struct MultiTypeHandler
{
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// pair											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType1 , typename ContainedType2 >
class MultiTypeSerializer< std::pair< ContainedType1 , ContainedType2 > > : public Serializable
{
	private	: ContainedType1 first;
	private	: ContainedType2 second;

	public : MultiTypeSerializer() {};
	public : virtual ~MultiTypeSerializer() {};
	public 	: static string myName(string name="")
	{
		static string sname = "";
		if(name.size() != 0) sname = name;
		return sname;
	}
	public 	: virtual string getClassName() const
	{
		return myName();
	};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
	};
	public : void deserialize(any& a)
	{
		std::pair< ContainedType1 , ContainedType2 > * multiType
			= any_cast< std::pair< ContainedType1 , ContainedType2 > * >(a);

		multiType->first	= first;
		multiType->second	= second;
	};

	public : void serialize(any& a)
	{
		std::pair< ContainedType1 , ContainedType2 > * multiType
			= any_cast< std::pair< ContainedType1 , ContainedType2 > * >(a);

		first 			= multiType->first;
		second			= multiType->second;
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType1 , typename ContainedType2 >
struct MultiTypeHandler< std::pair< ContainedType1 , ContainedType2 > >
{
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()
	{
		return boost::shared_ptr< MultiTypeSerializer< std::pair< ContainedType1 , ContainedType2 > > >
			( new MultiTypeSerializer< std::pair< ContainedType1 , ContainedType2 > > );
	}
	static Factorable* CreateSMultiType()
	{
		return new MultiTypeSerializer< std::pair< ContainedType1 , ContainedType2 > >;
	}
	static void * CreatePureCustomMultiType()
	{
		return new std::pair< ContainedType1 , ContainedType2 >;
	}
	static const type_info& VerifyMultiType()
	{
		return typeid( std::pair< ContainedType1 , ContainedType2 > );
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ContainedType1 , typename ContainedType2 >
SerializableTypes::Type findType( std::pair< ContainedType1 , ContainedType2 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	ContainedType2 tmp2;
	bool fundamental2;
	findType(tmp2 , fundamental2 , str);

	fundamental = fundamental1 && fundamental2 ;

	// AFAIK typeid().name() guarantees to return unique name, which is exactly what I want (it can be different on different architectures, and can be different from one program execution to another - I don't care)

	string name = string("pair") + typeid( std::pair< ContainedType1 , ContainedType2 > ).name();
	string sname = "S" + name;

	MultiTypeSerializer< std::pair< ContainedType1 , ContainedType2 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<std::pair< ContainedType1 , ContainedType2 > >::CreateSMultiType ,
		MultiTypeHandler<std::pair< ContainedType1 , ContainedType2 > >::CreateSharedSMultiType ,
		MultiTypeHandler<std::pair< ContainedType1 , ContainedType2 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<std::pair< ContainedType1 , ContainedType2 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,
		fundamental );

	str = sname;
	return SerializableTypes::CUSTOM_CLASS;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// tuple 1											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 >
class MultiTypeSerializer< boost::tuple< ContainedType1 > > : public Serializable
{
	private	: ContainedType1 first;

	public : MultiTypeSerializer() {};
	public : virtual ~MultiTypeSerializer() {};

	public 	: static string myName(string name="")
	{
		static string sname = "";
		if(name.size() != 0) sname = name;
		return sname;
	}
	public 	: virtual string getClassName() const {return myName();};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(first);
	};
	public : void deserialize(any& a)
	{
		boost::tuple< ContainedType1 > * multiType
			= any_cast< boost::tuple< ContainedType1 > * >(a);

		multiType->get<0>()	= first;
	};
	public : void serialize(any& a)
	{
		boost::tuple< ContainedType1 > * multiType
			= any_cast< boost::tuple< ContainedType1 > * >(a);

		first 			= multiType->get<0>();
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 >
struct MultiTypeHandler< boost::tuple< ContainedType1 > >
{
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()
	{
		return boost::shared_ptr< MultiTypeSerializer< boost::tuple< ContainedType1 > > >
			( new MultiTypeSerializer< boost::tuple< ContainedType1 > > );
	}
	static Factorable* CreateSMultiType()
	{
		return new MultiTypeSerializer< boost::tuple< ContainedType1 > >;
	}
	static void * CreatePureCustomMultiType()
	{
		return new boost::tuple< ContainedType1 >;
	}
	static const type_info& VerifyMultiType()
	{
		return typeid( boost::tuple< ContainedType1 > );
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1  >
SerializableTypes::Type findType( boost::tuple< ContainedType1 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	fundamental = fundamental1;

	string name = string("tuple") + typeid( boost::tuple< ContainedType1 > ).name();
	string sname = "S" + name;

	MultiTypeSerializer< boost::tuple< ContainedType1 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 > >::CreateSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 > >::CreateSharedSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,
		fundamental );

	str = sname;
	return SerializableTypes::CUSTOM_CLASS;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// tuple 2											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 >
class MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 > > : public Serializable
{
	private	: ContainedType1 first;
	private : ContainedType2 second;

	public : MultiTypeSerializer() {};
	public : virtual ~MultiTypeSerializer() {};

	public 	: static string myName(string name="")
	{
		static string sname = "";
		if(name.size() != 0) sname = name;
		return sname;
	}
	public 	: virtual string getClassName() const {return myName();};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
	};
	public : void deserialize(any& a)
	{
		boost::tuple< ContainedType1 , ContainedType2 > * multiType
			= any_cast< boost::tuple< ContainedType1 , ContainedType2 > * >(a);

		multiType->get<0>()	= first;
		multiType->get<1>()	= second;
	};
	public : void serialize(any& a)
	{
		boost::tuple< ContainedType1 , ContainedType2 > * multiType
			= any_cast< boost::tuple< ContainedType1 , ContainedType2 > * >(a);

		first 			= multiType->get<0>();
		second 			= multiType->get<1>();
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 >
struct MultiTypeHandler< boost::tuple< ContainedType1 , ContainedType2 > >
{
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()
	{
		return boost::shared_ptr< MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 > > >
			( new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 > > );
	}
	static Factorable* CreateSMultiType()
	{
		return new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 > >;
	}
	static void * CreatePureCustomMultiType()
	{
		return new boost::tuple< ContainedType1 , ContainedType2 >;
	}
	static const type_info& VerifyMultiType()
	{
		return typeid( boost::tuple< ContainedType1 , ContainedType2 > );
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 >
SerializableTypes::Type findType( boost::tuple< ContainedType1 , ContainedType2 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	ContainedType2 tmp2;
	bool fundamental2;
	findType(tmp2 , fundamental2 , str);

	fundamental = fundamental1 && fundamental2;

	string name = string("tuple") + typeid( boost::tuple< ContainedType1 , ContainedType2 > ).name();
	string sname = "S" + name;

	MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 > >::CreateSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 > >::CreateSharedSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,
		fundamental );

	str = sname;
	return SerializableTypes::CUSTOM_CLASS;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// tuple 3											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 >
class MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > > : public Serializable
{
	private	: ContainedType1 first;
	private : ContainedType2 second;
	private : ContainedType3 third;

	public : MultiTypeSerializer() {};
	public : virtual ~MultiTypeSerializer() {};

	public 	: static string myName(string name="")
	{
		static string sname = "";
		if(name.size() != 0) sname = name;
		return sname;
	}
	public 	: virtual string getClassName() const {return myName();};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
		REGISTER_ATTRIBUTE(third);
	};
	public : void deserialize(any& a)
	{
		boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > * multiType
			= any_cast< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > * >(a);

		multiType->get<0>()	= first;
		multiType->get<1>()	= second;
		multiType->get<2>()	= third;
	};
	public : void serialize(any& a)
	{
		boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > * multiType
			= any_cast< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > * >(a);

		first 			= multiType->get<0>();
		second 			= multiType->get<1>();
		third 			= multiType->get<2>();
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 >
struct MultiTypeHandler< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >
{
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()
	{
		return boost::shared_ptr< MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > > >
			( new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > > );
	}
	static Factorable* CreateSMultiType()
	{
		return new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >;
	}
	static void * CreatePureCustomMultiType()
	{
		return new boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 >;
	}
	static const type_info& VerifyMultiType()
	{
		return typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > );
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 >
SerializableTypes::Type findType( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	ContainedType2 tmp2;
	bool fundamental2;
	findType(tmp2 , fundamental2 , str);

	ContainedType3 tmp3;
	bool fundamental3;
	findType(tmp3 , fundamental3 , str);

	fundamental = fundamental1 && fundamental2 && fundamental3;

	string name = string("tuple") + typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > ).name();
	string sname = "S" + name;

	MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >::CreateSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >::CreateSharedSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,
		fundamental );

	str = sname;
	return SerializableTypes::CUSTOM_CLASS;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// tuple 4											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 >
class MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > > : public Serializable
{
	private	: ContainedType1 first;
	private : ContainedType2 second;
	private : ContainedType3 third;
	private : ContainedType4 fourth;

	public : MultiTypeSerializer() {};				// lines marked '//' are similar in all handlers
	public : virtual ~MultiTypeSerializer() {};			//
	public 	: static string myName(string name="")			//
	{								//
		static string sname = "";				//
		if(name.size() != 0) sname = name;			//
		return sname;						//
	}								//
	public 	: virtual string getClassName() const {return myName();};	//
	public : void registerAttributes()				//
	{								//
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
		REGISTER_ATTRIBUTE(third);
		REGISTER_ATTRIBUTE(fourth);
	};								//
	public : void deserialize(any& a)				//
	{								//
		boost::tuple			< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > * multiType
		= any_cast< boost::tuple	< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > * >(a);

		multiType->get<0>()	= first;
		multiType->get<1>()	= second;
		multiType->get<2>()	= third;
		multiType->get<3>()	= fourth;
	};								//
	public : void serialize(any& a)					//
	{								//
		boost::tuple			< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > * multiType
		= any_cast< boost::tuple	< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > * >(a);

		first 			= multiType->get<0>();
		second 			= multiType->get<1>();
		third 			= multiType->get<2>();
		fourth 			= multiType->get<3>();
	};								//
};									//

///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 >
struct MultiTypeHandler< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >
{									//
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()	//
	{								//
		return boost::shared_ptr< MultiTypeSerializer		//
						< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > > >
		( new MultiTypeSerializer	< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > > );
	}								//
	static Factorable* CreateSMultiType()				//
	{								//
		return new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >;
	}								//
	static void * CreatePureCustomMultiType()			//
	{								//
		return new boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 >;
	}								//
	static const type_info& VerifyMultiType()			//
	{								//
		return typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > );
	}								//
};									//


///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 >
SerializableTypes::Type findType( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	ContainedType2 tmp2;
	bool fundamental2;
	findType(tmp2 , fundamental2 , str);

	ContainedType3 tmp3;
	bool fundamental3;
	findType(tmp3 , fundamental3 , str);

	ContainedType4 tmp4;
	bool fundamental4;
	findType(tmp4 , fundamental4 , str);

	fundamental = fundamental1 && fundamental2 && fundamental3 && fundamental4;

	string name = string("tuple") + typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > ).name();
	string sname = "S" + name;					//

	MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >::CreateSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >::CreateSharedSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,			//
		fundamental );

	str = sname;							//
	return SerializableTypes::CUSTOM_CLASS;				//
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// tuple 5											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 , typename ContainedType5 >
class MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > > : public Serializable
{
	private	: ContainedType1 first;
	private : ContainedType2 second;
	private : ContainedType3 third;
	private : ContainedType4 fourth;
	private : ContainedType5 fifth;

	public : MultiTypeSerializer() {};				// lines marked '//' are similar in all handlers
	public : virtual ~MultiTypeSerializer() {};			//
	public 	: static string myName(string name="")			//
	{								//
		static string sname = "";				//
		if(name.size() != 0) sname = name;			//
		return sname;						//
	}								//
	public 	: virtual string getClassName() const {return myName();};	//
	public : void registerAttributes()				//
	{								//
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
		REGISTER_ATTRIBUTE(third);
		REGISTER_ATTRIBUTE(fourth);
		REGISTER_ATTRIBUTE(fifth);
	};								//
	public : void deserialize(any& a)				//
	{								//
		boost::tuple			< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > * multiType
		= any_cast< boost::tuple	< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > * >(a);

		multiType->get<0>()	= first;
		multiType->get<1>()	= second;
		multiType->get<2>()	= third;
		multiType->get<3>()	= fourth;
		multiType->get<4>()	= fifth;
	};								//
	public : void serialize(any& a)					//
	{								//
		boost::tuple			< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > * multiType
		= any_cast< boost::tuple	< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > * >(a);

		first 			= multiType->get<0>();
		second 			= multiType->get<1>();
		third 			= multiType->get<2>();
		fourth 			= multiType->get<3>();
		fifth 			= multiType->get<4>();
	};								//
};									//

///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 , typename ContainedType5 >
struct MultiTypeHandler< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >
{									//
	static boost::shared_ptr< Factorable > CreateSharedSMultiType()	//
	{								//
		return boost::shared_ptr< MultiTypeSerializer		//
						< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > > >
		( new MultiTypeSerializer	< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > > );
	}								//
	static Factorable* CreateSMultiType()				//
	{								//
		return new MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >;
	}								//
	static void * CreatePureCustomMultiType()			//
	{								//
		return new boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 >;
	}								//
	static const type_info& VerifyMultiType()			//
	{								//
		return typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > );
	}								//
};									//


///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename ContainedType1 , typename ContainedType2 , typename ContainedType3 , typename ContainedType4 , typename ContainedType5 >
SerializableTypes::Type findType( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 >& ,bool& fundamental, string& str)
{
	ContainedType1 tmp1;
	bool fundamental1;
	findType(tmp1 , fundamental1 , str);

	ContainedType2 tmp2;
	bool fundamental2;
	findType(tmp2 , fundamental2 , str);

	ContainedType3 tmp3;
	bool fundamental3;
	findType(tmp3 , fundamental3 , str);

	ContainedType4 tmp4;
	bool fundamental4;
	findType(tmp4 , fundamental4 , str);

	ContainedType5 tmp5;
	bool fundamental5;
	findType(tmp5 , fundamental5 , str);

	fundamental = fundamental1 && fundamental2 && fundamental3 && fundamental4 && fundamental5;

	string name = string("tuple") + typeid( boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > ).name();
	string sname = "S" + name;					//

	MultiTypeSerializer< boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >::myName(sname);

	ClassFactory::instance().registerFactorable(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >::CreateSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >::CreateSharedSMultiType ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >::CreatePureCustomMultiType);

	SerializableSingleton::instance().registerSerializableDescriptor(
		sname ,
		MultiTypeHandler<boost::tuple< ContainedType1 , ContainedType2 , ContainedType3 , ContainedType4 , ContainedType5 > >::VerifyMultiType ,
		SerializableTypes::CUSTOM_CLASS ,			//
		fundamental );						//
									//
	str = sname;							//
	return SerializableTypes::CUSTOM_CLASS;				//
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __MULTITYPEHANDLER_H__
