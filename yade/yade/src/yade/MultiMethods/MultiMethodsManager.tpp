
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

#include "Indexable.hpp"
#include "MultiMethodsManagerExceptions.hpp"
#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class Functor>
MultiMethodsManager<Functor>::MultiMethodsManager ()
{
	callBacks.resize(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class Functor>
MultiMethodsManager<Functor>::~MultiMethodsManager ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


template<class Functor>
bool MultiMethodsManager<Functor>::add(shared_ptr<Indexable>& i1, shared_ptr<Indexable>& i2, const string& name1, const string& name2, const string& libName)
{
	int& index1 = i1->getClassIndex();
	
	if (index1==-1)
	{
		index1 = i1->getMaxCurrentlyUsedClassIndex()+1;
		i1->incrementMaxCurrentlyUsedClassIndex();
	}

	// if i1->is the same as i2->then index2!=-1 and we are on matrix diagonal
	int& index2 = i2->getClassIndex();
		
	if (index2==-1)
	{
		index2 = i2->getMaxCurrentlyUsedClassIndex()+1;
		i2->incrementMaxCurrentlyUsedClassIndex();
	}
	
	//assert(i1->getMaxCurrentlyUsedClassIndex()==i2->getMaxCurrentlyUsedClassIndex());
	
	int maxCurrentIndex = max(i1->getMaxCurrentlyUsedClassIndex(),i2->getMaxCurrentlyUsedClassIndex());

	// resizing callBacks table
	callBacks.resize( maxCurrentIndex+1 );
	
	typedef typename vector<vector<shared_ptr<Functor> > >::iterator FunctorIterator;
	FunctorIterator ci    = callBacks.begin();
	FunctorIterator ciEnd = callBacks.end();
	for( ; ci != ciEnd ; ++ci )
		(*ci).resize(maxCurrentIndex+1);

	shared_ptr<Functor> functor,reverseFunctor;
	try
	{
		functor = dynamic_pointer_cast<Functor>(ClassFactory::instance().createShared(libName));
		reverseFunctor = dynamic_pointer_cast<Functor>(ClassFactory::instance().createShared(libName));
	}
	catch (FactoryCantCreate& fe)
	{
		std::string error = MultiMethodsManagerExceptions::NotExistingLibrary + libName;	
		throw MultiMethodsManagerNotExistingLibrary(error.c_str());
	}	
	
	string functorOrder = name1+" "+name2;
	string reverseFunctorOrder = name2+" "+name1;

	if (functor->checkFunctorOrder(functorOrder))
	{
		functor->setReverse(true);
		reverseFunctor->setReverse(false);
	}
	else if (functor->checkFunctorOrder(reverseFunctorOrder))
	{
		functor->setReverse(false);
		reverseFunctor->setReverse(true);
	}
	else
	{
		std::string error = MultiMethodsManagerExceptions::UndefinedCollisionOrder + libName;		
		throw MultiMethodsManagerUndefinedCollisionOrder(error.c_str());
	}
		
	callBacks[index2][index1] = functor;
	callBacks[index1][index2] = reverseFunctor;

	#ifdef DEBUG
		cerr <<" New class added to MultiMethodsManager: " << libName << endl;
	#endif

	return true;
}
