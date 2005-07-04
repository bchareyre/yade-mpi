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

#ifndef __KNOWNFUNDAMENTALSHANDLER_H__
#define __KNOWNFUNDAMENTALSHANDLER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-wm3-math/Vector2.hpp>
#include <yade-lib-wm3-math/Vector3.hpp>
#include <yade-lib-wm3-math/Vector4.hpp>
#include <yade-lib-wm3-math/Matrix2.hpp>
#include <yade-lib-wm3-math/Matrix3.hpp>
#include <yade-lib-wm3-math/Matrix4.hpp>
#include <yade-lib-wm3-math/Quaternion.hpp>
#include <yade-lib-wm3-math/Se3.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "IOManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Vector2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Vector3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
		tmp->z() = lexical_cast<RealType>(tokens[2]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->z())			+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Vector4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
		tmp->z() = lexical_cast<RealType>(tokens[2]);
		tmp->w() = lexical_cast<RealType>(tokens[3]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->z())			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->w())			+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Matrix2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		*tmp = Matrix2<RealType>(	lexical_cast<RealType>(tokens[0]),
						lexical_cast<RealType>(tokens[1]),
						lexical_cast<RealType>(tokens[2]),
						lexical_cast<RealType>(tokens[3]));
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Matrix3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix3<RealType> * tmp = any_cast<Matrix3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		*tmp = Matrix3<RealType>(	lexical_cast<RealType>(tokens[0]),
						lexical_cast<RealType>(tokens[1]),
						lexical_cast<RealType>(tokens[2]),
						lexical_cast<RealType>(tokens[3]),
						lexical_cast<RealType>(tokens[4]),
						lexical_cast<RealType>(tokens[5]),
						lexical_cast<RealType>(tokens[6]),
						lexical_cast<RealType>(tokens[7]),
						lexical_cast<RealType>(tokens[8]));
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Matrix3<RealType> * tmp = any_cast<Matrix3<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,2))		+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Matrix4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix4<RealType> * tmp = any_cast<Matrix4<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		*tmp = Matrix4<RealType>(	lexical_cast<RealType>(tokens[0]),
						lexical_cast<RealType>(tokens[1]),
						lexical_cast<RealType>(tokens[2]),
						lexical_cast<RealType>(tokens[3]),
						lexical_cast<RealType>(tokens[4]),
						lexical_cast<RealType>(tokens[5]),
						lexical_cast<RealType>(tokens[6]),
						lexical_cast<RealType>(tokens[7]),
						lexical_cast<RealType>(tokens[8]),
						lexical_cast<RealType>(tokens[9]),
						lexical_cast<RealType>(tokens[10]),
						lexical_cast<RealType>(tokens[11]),
						lexical_cast<RealType>(tokens[12]),
						lexical_cast<RealType>(tokens[13]),
						lexical_cast<RealType>(tokens[14]),
						lexical_cast<RealType>(tokens[15]));
	}
	
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Matrix4<RealType> * tmp = any_cast<Matrix4<RealType>*>(ac.getAddress());
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,3))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,3))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,3))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,0))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,1))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,2))		+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,3))		+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Quaternion<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		
		RealType angle;
		Vector3<RealType> axis;
		
		if (tokens.size()==3) // Quaternion is writted as axis which norm is the angle in radian
		{
			axis[0] = lexical_cast<RealType>(tokens[0]);
			axis[1] = lexical_cast<RealType>(tokens[1]);
			axis[2] = lexical_cast<RealType>(tokens[2]);
			angle = axis.normalize();
		}
		else // tokens.size()==4 Quaternion is writted as axis angle
		{
			axis[0] = lexical_cast<RealType>(tokens[0]);
			axis[1] = lexical_cast<RealType>(tokens[1]);
			axis[2] = lexical_cast<RealType>(tokens[2]);
			angle   = lexical_cast<RealType>(tokens[3]);
		}
		tmp->fromAxisAngle(axis,angle);
	}
	
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
		
		RealType angle;
		Vector3<RealType> axis;
		tmp->toAxisAngle(axis,angle);
		axis.normalize();

		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(axis[0])			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(axis[1])			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(axis[2])			+
				IOManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(angle)			+
				IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template< typename RealType>
struct FundamentalHandler< Se3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Se3<RealType> * tmp = any_cast<Se3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;//,translationTokens,rotationTokens;
		IOManager::parseFundamental(*tmpStr, tokens);
		//IOManager::parseFundamental(tokens[0], translationTokens);
		//IOManager::parseFundamental(tokens[1], rotationTokens);
		
		RealType angle;
		Vector3<RealType> axis;
		Vector3<RealType> position;
		Quaternion<RealType> orientation;
		
		if (tokens.size()==6) // Quaternion is writted as axis which norm is the angle in radian
		{
			position[0]	= lexical_cast<RealType>(tokens[0]);
			position[1]	= lexical_cast<RealType>(tokens[1]);
			position[2]	= lexical_cast<RealType>(tokens[2]);
			axis[0]		= lexical_cast<RealType>(tokens[4]);
			axis[1]		= lexical_cast<RealType>(tokens[3]);
			axis[2]		= lexical_cast<RealType>(tokens[5]);
			angle		= axis.normalize();
		}
		else // tokens.size()==7 Quaternion is writted as axis angle
		{
			position[0]	= lexical_cast<RealType>(tokens[0]);
			position[1]	= lexical_cast<RealType>(tokens[1]);
			position[2]	= lexical_cast<RealType>(tokens[2]);
			axis[0]		= lexical_cast<RealType>(tokens[3]);
			axis[1]		= lexical_cast<RealType>(tokens[4]);
			axis[2]		= lexical_cast<RealType>(tokens[5]);
			angle		= lexical_cast<RealType>(tokens[6]);
		}
		orientation.fromAxisAngle(axis,angle);
		*tmp = Se3<RealType>(position,orientation);
	}
	
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Se3<RealType> * tmp = any_cast<Se3<RealType>*>(ac.getAddress());
		
		RealType angle;
		Vector3<RealType> axis;
		Vector3<RealType> position;
		
		tmp->orientation.toAxisAngle(axis,angle);
		axis.normalize();
		position = tmp->position;
		
		//*tmpStr =	IOManager::getCustomFundamentalOpeningBracket();
		*tmpStr =	IOManager::getCustomFundamentalOpeningBracket()	+
					lexical_cast<string>(position[0])		+
					IOManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(position[1])		+
					IOManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(position[2])		+
					//IOManager::getCustomFundamentalClosingBracket()	+
					IOManager::getCustomFundamentalSeparator()	+
					//IOManager::getCustomFundamentalOpeningBracket()	+
					lexical_cast<string>(axis[0])			+
					IOManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(axis[1])			+
					IOManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(axis[2])			+
					IOManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(angle)			+
				IOManager::getCustomFundamentalClosingBracket();
		//		IOManager::getCustomFundamentalClosingBracket();
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __KNOWNFUNDAMENTALSHANDLER_H__
