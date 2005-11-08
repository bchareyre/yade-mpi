/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __KNOWNFUNDAMENTALSHANDLER_H__
#define __KNOWNFUNDAMENTALSHANDLER_H__


#include <yade/yade-lib-wm3-math/Vector2.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Vector4.hpp>
#include <yade/yade-lib-wm3-math/Matrix2.hpp>
#include <yade/yade-lib-wm3-math/Matrix3.hpp>
#include <yade/yade-lib-wm3-math/Matrix4.hpp>
#include <yade/yade-lib-wm3-math/Quaternion.hpp>
#include <yade/yade-lib-wm3-math/Se3.hpp>

#include "IOFormatManager.hpp"


template< typename RealType>
struct FundamentalHandler< Vector2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Vector3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
		tmp->z() = lexical_cast<RealType>(tokens[2]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->z())			+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Vector4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
		tmp->x() = lexical_cast<RealType>(tokens[0]);
		tmp->y() = lexical_cast<RealType>(tokens[1]);
		tmp->z() = lexical_cast<RealType>(tokens[2]);
		tmp->w() = lexical_cast<RealType>(tokens[3]);
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(tmp->x())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->y())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->z())			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(tmp->w())			+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
		*tmp = Matrix2<RealType>(	lexical_cast<RealType>(tokens[0]),
						lexical_cast<RealType>(tokens[1]),
						lexical_cast<RealType>(tokens[2]),
						lexical_cast<RealType>(tokens[3]));
	}
	static void accessor(Archive& ac, any& a)
	{
		string * tmpStr = any_cast<string*>(a);
		Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix3<RealType> * tmp = any_cast<Matrix3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
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
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,2))		+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Matrix4<RealType> * tmp = any_cast<Matrix4<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
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
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>((*tmp)(0,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(0,3))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(1,3))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(2,3))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,0))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,1))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,2))		+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>((*tmp)(3,3))		+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Quaternion<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
		
		vector<string> tokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		
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

		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
				lexical_cast<string>(axis[0])			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(axis[1])			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(axis[2])			+
				IOFormatManager::getCustomFundamentalSeparator()	+
				lexical_cast<string>(angle)			+
				IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


template< typename RealType>
struct FundamentalHandler< Se3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		const string * tmpStr = any_cast<const string*>(a);
		Se3<RealType> * tmp = any_cast<Se3<RealType>*>(ac.getAddress());
		
		vector<string> tokens;//,translationTokens,rotationTokens;
		IOFormatManager::parseFundamental(*tmpStr, tokens);
		//IOFormatManager::parseFundamental(tokens[0], translationTokens);
		//IOFormatManager::parseFundamental(tokens[1], rotationTokens);
		
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
		
		//*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket();
		*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
					lexical_cast<string>(position[0])		+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(position[1])		+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(position[2])		+
					//IOFormatManager::getCustomFundamentalClosingBracket()	+
					IOFormatManager::getCustomFundamentalSeparator()	+
					//IOFormatManager::getCustomFundamentalOpeningBracket()	+
					lexical_cast<string>(axis[0])			+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(axis[1])			+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(axis[2])			+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(angle)			+
				IOFormatManager::getCustomFundamentalClosingBracket();
		//		IOFormatManager::getCustomFundamentalClosingBracket();
	}
};


#endif // __KNOWNFUNDAMENTALSHANDLER_H__
