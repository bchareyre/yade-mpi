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

#ifndef __ARCHIVESTYPES_HPP__
#define __ARCHIVESTYPES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/any.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Archive;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace ArchiveTypes
{
	/*! \var
	<ul>
		<li>FUNDAMENTAL		- class that is (de-)serialized into a single string with separator : int,float, vector<fundamental>, quaternion, octonion, vector3</li>
		<li>CUSTOM_CLASS	- class not derived from Serializable, including POD</li>
		<li>SERIALIZABLE	- class derived from Serializable </li>
		<li>POINTER	- shared_ptr, weak_ptr, auto_ptr </li>
		<li>CONTAINER	- vector, list, queue, map, pair </li>
	</ul>
	*/
	typedef enum 	{	FUNDAMENTAL,
				CUSTOM_CLASS,
				SERIALIZABLE,
				POINTER,
				CONTAINER,
			} RecordType;

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
	typedef int  (*NextArchiveFnPtr)  		(Archive& ac, shared_ptr<Archive>& nextAc, bool first);
	/*! Define a pointer to a function used for deserializing container */
	typedef void (*ResizeFnPtr)			(Archive& ac, int size);
	/*! Define a pointer to a function used for serializing pointer */
	typedef bool (*PointedArchiveFnPtr)		(Archive& ac, shared_ptr<Archive>& newAc);
	/*! Define a pointer to a function used for deserializing pointer */
	typedef void (*PointedNewArchiveFnPtr)	(Archive& ac, shared_ptr<Archive>& newAc , string typeStr);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ARCHIVESTYPES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
