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

#ifndef __MULTIMETHODSMANAGER_H__
#define __MULTIMETHODSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <set>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ClassFactory.hpp"
#include "Indexable.hpp"
#include "CollisionModel.hpp"
#include "Se3.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class Functor>
class MultiMethodsManager 
{	
	protected : std::vector<std::vector<Functor> > callBacks;
	//private   : int size;
	private   : map<string,int> indexedClassName;
	
	// FIXME : remove nullFunc
	protected : Functor nullFunc;
		
	// construction
	public : MultiMethodsManager<Functor> ()
	{
		callBacks.resize(0);
	};

	public : virtual ~MultiMethodsManager () {};

	public : void add(const string& name)
	{
		if (indexedClassName.find(name)!=indexedClassName.end())
		{			
			shared_ptr<Indexable> indexable = shared_dynamic_cast<Indexable>(ClassFactory::instance().createShared(name));
			int& index = indexable->getClassIndex();
			assert(index==-1);
			index = indexedClassName.size()-1;
			indexedClassName[name] = index;
			map<string,int>::iterator icni    = indexedClassName.begin();
			map<string,int>::iterator icniEnd = indexedClassName.end();
			for( ;icni!=icniEnd; ++icni )
			{
				string functorName = name+"2"+(*icni).first+"4ClosestFeatures";
				string reverseFunctorName = (*icni).first+"2"+name+"4ClosestFeatures";
				shared_ptr<Functor> collisionFunctor,reverseCollisionFunctor;
				try
				{
					collisionFunctor = shared_dynamic_cast<Functor>(ClassFactory::instance().createShared(functorName));
					reverseCollisionFunctor = shared_dynamic_cast<Functor>(ClassFactory::instance().createShared(functorName));
					collisionFunctor->setReverse(false);
					reverseCollisionFunctor->setReverse(true);
				}
				catch (FactoryError& fe)
				{
					collisionFunctor = shared_dynamic_cast<Functor>(ClassFactory::instance().createShared(reverseFunctorName));
					reverseCollisionFunctor = shared_dynamic_cast<Functor>(ClassFactory::instance().createShared(reverseFunctorName));
					collisionFunctor->setReverse(true);
					reverseCollisionFunctor->setReverse(false);	
				}
				
				callBacks[index][indexedClassName[(*icni).first]] = collisionFunctor;
				callBacks[indexedClassName[(*icni).first]][index] = reverseCollisionFunctor;
			}
		}
	}
	
	/*private : bool pairExists(int i,int j)
	{
		int sizeRow = callBacks.size();
		int max = i;
		if (max<j)
			max = j;	
		if (max>=sizeRow)
			return false;
		else
			return (callBacks[i][j]!=nullFunc && callBacks[j][i]!=nullFunc);
	}
	
	protected : bool addPair(int i,int j, Functor ptr2F1,Functor ptr2F2)
	{
		//assert(ptf1!=NULL);
		if (!pairExists(i,j))
		{	
			int sizeRow = callBacks.size();
			int max = i;
			if (max<j)
				max = j;
			if (sizeRow <= max)
			{
				callBacks.resize(max+1);
				for(int k=0;k<=max;k++)
				{
					callBacks[k].resize(max+1);
					for(int l=0;l<=max;l++)
					{					
						if (k>=max || l>=max)
							callBacks[k][l] = nullFunc;
					}
				}
			}
			callBacks[i][j] = ptr2F1;
			if (i!=j)
				callBacks[j][i] = ptr2F2;
			return true;
		}
		else
			return false;
	}
	
	protected : bool overwritePair(int i,int j, Functor ptr2F1,Functor ptr2F2)
		    {
			if (pairExists(i,j))
			{
				callBacks[i][j] = ptr2F1;
				if (i!=j)
					callBacks[j][i] = ptr2F2;
				return true;
			}
			else
				return false;
		    }
	
	protected : bool removePair(int i,int j)
	{
		if (pairExists(i,j))
		{
			callBacks[i][j] = nullFunc;
			callBacks[j][i] = nullFunc;
			return true;
		}
		else
			return false;
	}*/

	public : inline bool operator() (const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c)
	{
		assert(cm1->getClassIndex()>0);
		assert(cm2->getClassIndex()>0);
		assert(cm1->getClassIndex()<callBacks.size());
		assert(cm2->getClassIndex()<callBacks.size());
		shared_ptr<Functor> cf = callBacks[cm1->getClassIndex()][cm2->getClassIndex()];
		if (cf!=0)
			return (*cf)(cm1,cm2,se31,se32,c);
		else
			return false;
	}
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __MULTIMETHODSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
