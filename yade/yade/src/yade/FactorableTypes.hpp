/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *   Copyright (C) 2005 by Janek Kozicki                                   *
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

#ifndef __FACTORABLETYPES_HPP__
#define __FACTORABLETYPES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace FactorableTypes
{
	/*! \var
	<ul>
		<li>FACTORABLE		- class derived from Factorable </li>
		<li>SERIALIZABLE	- class derived from Serializable derived from Factorable </li>
		<li>FUNDAMENTAL		- class Serializable derived from Factorable class that is
					  (de-)serialized into a single string with separator : int,float, vector<fundamental>,
					  quaternion, octonion, vector3</li>
		<li>POINTER		- component of Serialization: shared_ptr, weak_ptr, auto_ptr, ... </li>
		<li>CONTAINER		- component of Serialization: vector, list, queue, map, pair, ... </li>
		<li>CUSTOM_CLASS	- Serializable class not derived from Serializable, including POD</li>
	</ul>
	*/
	typedef enum
		{
			FACTORABLE,
			SERIALIZABLE,
			FUNDAMENTAL,
			POINTER,
			CONTAINER,
			CUSTOM_CLASS
		} Type;


}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __FACTORABLETYPES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
