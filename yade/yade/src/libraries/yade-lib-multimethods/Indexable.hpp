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

#include <boost/scoped_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all Indexable class.

	An indexable class is a class that will be managed by a MultiMethodManager. The index the function getClassIndex() returns, corresponds to the index in the matrix where the class will be handled.
*/
class Indexable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : Indexable ();

	/*! Destructor */
	public : virtual ~Indexable ();

	protected : void createIndex();
	
	/*! Returns the id of the current class. This id is set by a multimethod manager */
	public : virtual int& getClassIndex() { throw;};
	public : virtual const int& getClassIndex() const { throw;};
	public : virtual int& getBaseClassIndex(int ) { throw;};
	public : virtual const int& getBaseClassIndex(int ) const { throw;};
	
	public : virtual const int& getMaxCurrentlyUsedClassIndex() const { throw;};
	public : virtual void incrementMaxCurrentlyUsedClassIndex() { throw;};

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// this macro is used by classes that are a dimension in multimethod matrix

// this assert was removed, because I must ask Base class for it's own index.
//		assert(typeid(*this)==typeid(SomeClass));

#define REGISTER_CLASS_INDEX(SomeClass,BaseClass)				\
	private: static int& getClassIndexStatic()				\
	{									\
		static int index = -1;						\
		return index;							\
	}									\
	public: virtual int& getClassIndex()					\
	{									\
		return getClassIndexStatic();					\
	}									\
	public: virtual const int& getClassIndex() const			\
	{									\
		return getClassIndexStatic();					\
	}									\
										\
										\
	public: virtual int& getBaseClassIndex(int depth)			\
	{									\
		static boost::scoped_ptr<BaseClass> baseClass(new BaseClass);	\
		if(depth == 1)							\
			return baseClass->getClassIndex();			\
		else								\
			return baseClass->getBaseClassIndex(--depth);		\
	}									\
	public: virtual const int& getBaseClassIndex(int depth) const		\
	{									\
		static boost::scoped_ptr<BaseClass> baseClass(new BaseClass);	\
		if(depth == 1)							\
			return baseClass->getClassIndex();			\
		else								\
			return baseClass->getBaseClassIndex(--depth);		\
	}


// this macro is used by base class for classes that are a dimension in multimethod matrix
// to keep track of maximum number of classes of their kin. Multimethod matrix can't
// count this number (ie. as a size of the matrix) - because there are many multimethod matrices!

#define REGISTER_INDEX_COUNTER(SomeClass)					\
										\
	private: static int& getClassIndexStatic()				\
	{									\
		static int index = -1;						\
		return index;							\
	}									\
	public: virtual int& getClassIndex()					\
	{									\
		return getClassIndexStatic();					\
	}									\
	public: virtual const int& getClassIndex() const			\
	{									\
		return getClassIndexStatic();					\
	}									\
	public: virtual int& getBaseClassIndex(int)				\
	{									\
		throw;								\
	}									\
	public: virtual const int& getBaseClassIndex(int) const			\
	{									\
		throw;								\
	}									\
										\
										\
	private: static int& getMaxCurrentlyUsedIndexStatic()			\
	{									\
		static int maxCurrentlyUsedIndex = -1;				\
		return maxCurrentlyUsedIndex;					\
	}									\
	public: virtual const int& getMaxCurrentlyUsedClassIndex() const	\
	{									\
		SomeClass * Indexable##SomeClass = 0;				\
		Indexable##SomeClass = dynamic_cast<SomeClass*>(const_cast<SomeClass*>(this));		\
		assert(Indexable##SomeClass);					\
		return getMaxCurrentlyUsedIndexStatic();			\
	}									\
	public: virtual void incrementMaxCurrentlyUsedClassIndex()		\
	{									\
		SomeClass * Indexable##SomeClass = 0;				\
		Indexable##SomeClass = dynamic_cast<SomeClass*>(this);		\
		assert(Indexable##SomeClass);					\
		int& max = getMaxCurrentlyUsedIndexStatic();			\
		max++;								\
	}									\

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INDEXABLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

