/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INDEXABLE_HPP
#define INDEXABLE_HPP

#include <boost/scoped_ptr.hpp>

/*! \brief Abstract interface for all Indexable class.
	An indexable class is a class that will be managed by a MultiMethodManager.
	The index the function getClassIndex() returns, corresponds to the index in the matrix where the class will be handled.
*/

class Indexable
{
	protected :
		void createIndex();

	public :
		Indexable ();
		virtual ~Indexable ();

		virtual int& getClassIndex() { throw;};  /// Returns the id of the current class. This id is set by a multimethod manager
		virtual const int& getClassIndex() const { throw;};
		virtual int& getBaseClassIndex(int ) { throw;};
		virtual const int& getBaseClassIndex(int ) const { throw;};
		
		virtual const int& getMaxCurrentlyUsedClassIndex() const { throw;};
		virtual void incrementMaxCurrentlyUsedClassIndex() { throw;};

};

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

#endif // INDEXABLE_HPP

