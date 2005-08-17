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

#ifndef __ARCHIVECONTENT_HPP__
#define __ARCHIVECONTENT_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SerializationExceptions.hpp"
#include "ArchiveTypes.hpp"
#include "SerializableTypes.hpp"
#include "SerializableSingleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;
using namespace std;
using namespace ArchiveTypes;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME - if you are doing major rewrite of Serialization, check if following macro
// could make good use of Loki::OrderedTypeInfo

#define REGISTER_SERIALIZABLE_DESCRIPTOR(name,sname,type,isFundamental) 		\
	inline const type_info& Verify##name()						\
	{										\
		return typeid(name);							\
	}										\
	const bool registered##name##sname =						\
		SerializableSingleton::instance().registerSerializableDescriptor(	\
								#sname ,		\
								Verify##name ,		\
								type,			\
								isFundamental );

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract representation of a class attributes.

	This class is designed for serialization purpose. For each attribute of a class to serialize, an Archive
	is build, based on its <a href="../ArchiveTypes.html">type</a>. Once this archive is built it is possible
	to write it down to a stream (e.g. a file) of any type (XML, YAML, GNUPLOT ....) via a dynamic library
	derived from <a href="../IOFormatManager.html">IOFormatManager</a>. This not on the fly serialization process using
	an abstract representation an data gives the possibility to be independant of the file type and so to
	create a new one without recompiling anything.
	\warning You will need to use this class only if you want to create your own IOFormatManager
	
	All this serialization stuff needs rewriting, especially class Archive is a total mess. Rewriting should
	make it clean, while preserving original client interface.
*/
class Archive
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Pointer to a function that build the archive of the next element in the container. So it means
	that the current archive represent a attribute that is a container (std::vector ....)
		\param ac archive of the container to serialize
		\param nextAc archive of the next element in the container
		\param first is true if you call this function for the first time when serializing a cointainer
		\return the number of element of the container
		\warning used only if recordType=CONTAINER and while serializing
		\see ContainerHandler.hpp
	*/
	public    : NextArchiveFnPtr createNextArchive;

	/*! Pointer to a function that resize the container stored into ac
		\param ac the archive that contains the container to resize
		\par
		\warning used only if recordType=CONTAINER and while deserializing
		\see ContainerHandler.hpp
	*/
	public    : ResizeFnPtr resize;

	/*! Pointer to a function that return an archive that represents the pointed element
		\param ac the archive that contains the pointer
		\param newAc the archive representing the pointed element
		\warning used only if recordType=POINTER and while serializing
		\see PointerHandler.hpp
	*/
	public    : PointedArchiveFnPtr createPointedArchive;

	/*! Pointer to a function that create an archive that represents the pointed element
		\param ac the archive that contains the pointer
		\param newAc the archive representing the pointed element
		\warning used only if recordType=POINTER and while deserializing
		\see PointerHandler.hpp
	*/
	public    : PointedNewArchiveFnPtr createNewPointedArchive;

	/*! Pointer to a function that convert a fundamental type (int,float,string...) to
	a string (for text output format) or vector<unsigned char> (for binary output format)
		\param ac the Archive representing the fundamental attribute
		\param a  contains a pointer to the string or vector<unsigned char> that will be filled
		\warning used only if recordType=FUNDAMENTAL and while serializing
		\see FundamentalHandler.hpp
	*/
	public    : SerializeFundamentalFnPtr serializeFundamental;

	/*! Pointer to a function that convert a the string or vector<unsigned char> to a fundamental
	type (int,float,string...)
		\param ac the Archive to fill
		\param a  contains a pointer to the string or vector<unsigned char> to deserialize
		\warning used only if recordType=FUNDAMENTAL and while deserializing
		\see FundamentalHandler.hpp
	*/
	public    : DeserializeFundamentalFnPtr deserializeFundamental;

	/*! Function pointer assigned by the create function. Contains a pointer to a function that is able to
	serialize the current recordType type of attribute */
	public    : SerializeFnPtr serialize;

	/*! Function pointer assigned by the create function. Contains a pointer to a function that is able to
	deserialize the current recordType type of attribute */
	public    : DeserializeFnPtr deserialize;

	/*! This attribute is valid only if the current archive is of type CUSTOM_CLASS. It contains the name
	of the serializable version of the class. */
	private   : string serializableClassName;

	/*! Contains a pointer to the attribute	*/
	private : any address;

	/*! Name of the attribute */
	private   : string name;

	/*! fundamental is true if the archive represent a fundamental type (int,float,
	vector<int>, shared_pointer<int>, vector<shared_ptr<Quaternion>> */
	private   : bool fundamental;

	/*! processed is true if the atrtibute represented by the current archive has already been (de)-serialized */
	private   : bool processed;

	/*! Stl map that contains a pointer to the serialization and deserialization function for each non
	fundamental type */
	private   : static map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > serializationMap;

	/*! Stl map that contains a pointer to the serialization and deserialization function for each non
	fundamental type */
	private   : static map<SerializableTypes::Type,pair<SerializeFnPtr,DeserializeFnPtr> > serializationMapOfFundamental;

	/*! <a href="../ArchiveTypes.html">Type</a> of the attribute represented by the current archive */
	private   : SerializableTypes::Type recordType;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! @name Constructor/Destructor function*/
//@{
	/*! Constructor
	\see create()
	\warning This constructor is only used in the create function
	*/
	public    : explicit Archive(const string& n);

	/*! Destructor
	\note do nothing
	*/
	public    : virtual ~Archive();

	/*! Factory used to construct an Archive object.
	\param name Name of the attributes
	\param attribute the attribute from which the Archive will be constructed
	\return a pointer to the constructed archive
	*/
	public    : template<typename Type>
	 	    static inline shared_ptr<Archive> create(const string& name,Type& attribute);
//@}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Method											///
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! @name Getters and setters*/
//@{
	/*! Getter for the recordType attribute*/
	public	  : inline SerializableTypes::Type getRecordType() {return recordType;};

	/*! Setter for the recordType attribute*/
	public	  : inline void setRecordType(SerializableTypes::Type a) {recordType=a;};

	/*! Setter for the serializableClassName attribute*/
	public    : void setSerializableClassName(const string& s) {serializableClassName=s;};

	/*! Getter for the serializableClassName attribute*/
	public    : string& getSerializableClassName() { return serializableClassName;};

	/*! Getter for the address attribute */
	public    : any& getAddress() { return address;};

	/*! Setter for the address attribute */
	public    : void setAddress(any a) { address=a;};

	/*! Getter for the name attribute */
	public    : string getName() {return name;};

	/*! Setter for the fundamental attribute */
	public    : void setFundamental(bool f) { fundamental = f;};

	/*! Getter for the fundamental attribute */
	public    : bool isFundamental() { return fundamental;};

	/*! Setter for the processed attribute */
	public    : void markProcessed() {processed = true;};

	/*! Getter for the processed attribute */
	public    : bool isProcessed() { return processed;};
//@}

	/*! Returns true is the archive is an archive of a fundamental type or of a class that contains
	only fundamental types.	This is useful for XML output for example that can write simple attributes in a different
	way than write complex one.
	\return true is the current archive is a fundamental archive or if it is an archive of a custom	or serializable class
	that contains only fundamentals.
	*/
	public    : bool containsOnlyFundamentals();

	/*! This function is used to fill the maps serializationMap and serializationMapOfFundamental
	You should call it only from your own IOFormatManager to tell the Archive which function to use for
	(de)-serializing a given type
	\param rt Type attribute that the function sp and dsp are designed to (de)-serialized
	\param fundamental is true if the function sp and dsp are designed to (de)-serialized a fundamental type
	that is to say to are designed to read and write from a single string (or vector<unsigned char>)
	\param sp pointer to the serialization function for type rt that will be stored into the map
	\param dsp pointer to the deserialization function for type rt that will be stored into the map
	*/
	public    : static bool addSerializablePointer(SerializableTypes::Type rt ,bool fundamental, SerializeFnPtr sp, DeserializeFnPtr dsp);




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
	private   : static SerializableDescriptorMap map;
	public    : static bool registerSerializableDescriptor( string name, VerifyFactorableFnPtr verify, SerializableTypes::Type type, bool f);
	public    : static bool findClassInfo(const type_info& tp,SerializableTypes::Type& type, string& serializableClassName,bool& fundamental);



};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Archive.tpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ARCHIVECONTENT_HPP__
