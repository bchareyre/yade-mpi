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

#ifndef __INDEXABLE_H__
#define __INDEXABLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//#include "MultiMethodsManager.hpp"
#include "Omega.hpp"
//#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all Indexable class.

	An indexable class is a class that will be managed by a MultiMethodManager. The index the function getClassIndex() returns, corresponds to the index in the matrix where the class will be handled.
*/
//FIXME : is indexable serializable ????
class Indexable// : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : Indexable () {};

	/*! Destructor */
	public : virtual ~Indexable () {};

	/*! Returns the id of the current class. This id is set by a multimethod manager */
	public : virtual int& getClassIndex() { throw;};
	public : virtual const int& getClassIndex() const { throw;};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
   
#define REGISTER_CLASS_TO_MULTI_METHODS_MANAGER(SomeClass)	\
	const bool indexed##SomeClass = Omega::instance().narrowCollider.add(#SomeClass);\
	
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_CLASS_INDEX(SomeClass)				\
	static int& getClassIndexStatic()			\
	{							\
		static int index = -1;				\
		return index;					\
	}							\
	virtual int& getClassIndex()				\
	{							\
		assert(typeid(*this)==typeid(SomeClass));	\
		return getClassIndexStatic();			\
	}							\
	virtual const int& getClassIndex() const		\
	{							\
		assert(typeid(*this)==typeid(SomeClass));	\
		return getClassIndexStatic();			\
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INDEXABLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
