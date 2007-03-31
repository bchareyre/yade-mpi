/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __SERIALIZABLETYPES_HPP__
#define __SERIALIZABLETYPES_HPP__

namespace SerializableTypes
{
	typedef enum
		{
			SERIALIZABLE,   	/// class derived from Serializable derived from Factorable 
			FUNDAMENTAL,    	/// class Serializable derived from Factorable class that is
                                        	///   (de-)serialized into a single string with separator : int,float, vector<fundamental>,
                                        	///   quaternion, octonion, vector3
			POINTER,        	/// component of Serialization: shared_ptr, weak_ptr, auto_ptr, ...
			CONTAINER,      	/// component of Serialization: vector, list, queue, map, pair, ...
			CUSTOM_CLASS    	/// Serializable class not derived from Serializable, including POD
		} Type;
}

#endif // __SERIALIZABLETYPES_HPP__

