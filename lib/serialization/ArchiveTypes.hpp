/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once


#include <boost/any.hpp>
#ifndef  __GXX_EXPERIMENTAL_CXX0X__
#	include<boost/shared_ptr.hpp>
	using boost::shared_ptr;
#else
#	include<memory>
	using std::shared_ptr;
#endif


using namespace boost;

class Archive;

namespace ArchiveTypes
{
	/*! Define a pointer to a function that will serialize into a stream "stream" the content of an archive
	"ac". Depth is used for recursive file format like XML */
	typedef void (*SerializeFnPtr)   (ostream& stream, Archive& ac , int depth);

	// FIXME : be careful const string& is only for text file => but it is maybe possible to use it for binary file
	/*! Define a pointer to a function that will deserialize from a stream "stream" the content of an archive
	"ac". The parameter "str" is used for fundamental archive only which are type that can be stored into a single string.*/
	typedef void (*DeserializeFnPtr) (istream& stream, Archive& ac , const string& str);

	/*! Define a pointer to a function used for serializing fundamentals */
	typedef void (*SerializeFundamentalFnPtr)	(Archive& ac, any& a);

	/*! Define a pointer to a function used for deserializing fundamentals */
	typedef void (*DeserializeFundamentalFnPtr)	(Archive& ac, any& a);

	/*! Define a pointer to a function used for serializing container */
	typedef int  (*NextArchiveFnPtr)  		(Archive& ac, shared_ptr<Archive>& nextAc, bool first );

	/*! Define a pointer to a function used for deserializing container */
	typedef void (*ResizeFnPtr)			(Archive& ac, unsigned long int size);

	/*! Define a pointer to a function used for serializing pointer */
	typedef bool (*PointedArchiveFnPtr)		(Archive& ac, shared_ptr<Archive>& newAc);

	/*! Define a pointer to a function used for deserializing pointer */
	typedef void (*PointedNewArchiveFnPtr)	(Archive& ac, shared_ptr<Archive>& newAc , string typeStr);

}


