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
//FIXME : hpp => tpp
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CONTAINERHANDLER_H__
#define __CONTAINERHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ExceptionMessages.hpp"
#include "Archive.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Type>
struct ContainerHandler
{
	static void resize(Archive& , int )
	{
		throw HandlerError(ExceptionMessages::ContainerNotSupported);
	}

	static int accessNext(Archive& , shared_ptr<Archive>& , bool )
	{
		throw HandlerError(ExceptionMessages::ContainerNotSupported);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// vector											///
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename VectoredType>
struct ContainerHandler<std::vector<VectoredType> >
{
	static void resize(Archive& ac, int size)
	{
		vector<VectoredType> * tmpVec;
		tmpVec=any_cast<vector<VectoredType>*>(ac.getAddress());
		tmpVec->resize(size);
	}

	static int accessNext(Archive& ac, shared_ptr<Archive>& nextAc , bool first)
	{
		typedef typename vector<VectoredType>::iterator VectorIterator;
		static VectorIterator it;
		static VectorIterator itEnd;
		static int i;

		vector<VectoredType> * tmpVec;
		tmpVec=any_cast<vector<VectoredType>*>(ac.getAddress());

		if(first)
		{
			it=tmpVec->begin();
			itEnd=tmpVec->end();
			i=0;
		}

		if(it != itEnd )
		{
			string name = ac.getName()+"["+lexical_cast<string>(i)+"]";
			nextAc = Archive::create(name,(*it));
			++it;
			++i;
			return tmpVec->size();
		}
		else
			return 0;

	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename VectoredType>
FactorableTypes::Type findType( std::vector<VectoredType>& ,bool& fundamental, string& str)
{
// 	VectoredType tmpV;
//
// 	if (	boost::is_fundamental<VectoredType>::value ||
// 		findType(tmpV,str) == SMART_POINTER_OF_FUNDAMENTAL ||
// 		findType(tmpV,str) == CUSTOM_FUNDAMENTAL
// 		|| findType(tmpV,str) == STL_CONTAINER_OF_FUNDAMENTAL)
// 		return STL_CONTAINER_OF_FUNDAMENTAL;
// 	else
// 		return CONTAINER;

	VectoredType tmpV;
	//bool tmpFundamental;

	//FIXME : tmpFundamental ??
	//RecordType t = findType(tmpV,tmpFundamental,str);
	//cerr << str << " " << tmpFundamental << endl;
	//fundamental = ( boost::is_fundamental<VectoredType>::value || ((t==POINTER || t==CUSTOM_CLASS || t==CONTAINER) && tmpFundamental) );

	//RecordType t =
	findType(tmpV,fundamental,str);

	return FactorableTypes::CONTAINER;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __CONTAINERHANDLER_H__
