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

#ifndef __N2NPTR2FUNCLOOKUPTABLE_H__
#define __N2NPTR2FUNCLOOKUPTABLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class ptr2Func>
class N2NPtr2FuncLookUpTable 
{	
	protected : std::vector<std::vector<ptr2Func> > lookUpTable;
	
	protected : ptr2Func nullFunc;
		
	// construction
	public : N2NPtr2FuncLookUpTable<ptr2Func> ()
	{
		lookUpTable.resize(0);
	};

	public : virtual ~N2NPtr2FuncLookUpTable () {};
	
	private : bool pairExists(int i,int j)
	{
		int sizeRow = lookUpTable.size();
	int max = i;
	if (max<j)
		max = j;	
	if (max>=sizeRow)
		return false;
	else
		return (lookUpTable[i][j]!=nullFunc && lookUpTable[j][i]!=nullFunc);
	}
	
	protected : bool addPair(int i,int j, ptr2Func ptr2F1,ptr2Func ptr2F2)
	{
		//assert(ptf1!=NULL);
		if (!pairExists(i,j))
		{	
			int sizeRow = lookUpTable.size();
			int max = i;
			if (max<j)
				max = j;
			if (sizeRow <= max)
			{
				lookUpTable.resize(max+1);
				for(int k=0;k<=max;k++)
				{
					lookUpTable[k].resize(max+1);
					for(int l=0;l<=max;l++)
					{					
						if (k>=max || l>=max)
							lookUpTable[k][l] = nullFunc;
					}
				}
			}
			lookUpTable[i][j] = ptr2F1;
			if (i!=j)
				lookUpTable[j][i] = ptr2F2;
			return true;
		}
		else
			return false;
	}
	
	protected : bool overwritePair(int i,int j, ptr2Func ptr2F1,ptr2Func ptr2F2)
		    {
			if (pairExists(i,j))
			{
				lookUpTable[i][j] = ptr2F1;
				if (i!=j)
					lookUpTable[j][i] = ptr2F2;
				return true;
			}
			else
				return false;
		    }
	
	protected : bool removePair(int i,int j)
	{
		if (pairExists(i,j))
		{
			lookUpTable[i][j] = nullFunc;
			lookUpTable[j][i] = nullFunc;
			return true;
		}
		else
			return false;
	}
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __N2NPTR2FUNCLOOKUPTABLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
