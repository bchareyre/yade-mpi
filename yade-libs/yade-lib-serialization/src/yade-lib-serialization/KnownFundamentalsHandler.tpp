/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef KNOWNFUNDAMENTALSHANDLER_HPP
#define KNOWNFUNDAMENTALSHANDLER_HPP


#include <yade/yade-lib-wm3-math/Vector2.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Vector4.hpp>
#include <yade/yade-lib-wm3-math/Matrix2.hpp>
#include <yade/yade-lib-wm3-math/Matrix3.hpp>
#include <yade/yade-lib-wm3-math/Matrix4.hpp>
#include <yade/yade-lib-wm3-math/Quaternion.hpp>
#include <yade/yade-lib-wm3-math/Se3.hpp>

#include "IOFormatManager.hpp"

template<typename Type >
inline void binary_to_data(const vector<unsigned char>& bin, vector<Type>& data)
{
	// in first element of const vector<unsigned char> bin we have 8 bits: xxxxyyyy
	// xxxx is number of elements in vector<Type> data-1 (so max is 16)
	// yyyy is sizeof(Type) of each element-1 (so max is 16)
	unsigned char size3 = bin[0]; 			// packed data
	unsigned char size2 = size3 & 0x0f; 		// sizeof(Type) - 1
	unsigned char size  = size3 >> 4; 		// nuber of records - 1
	++size2;
	assert(size2 == sizeof(Type));
	static Type tmp;
	for(unsigned char i = 0 ; i<=size ; ++i)
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(&tmp);
		for(int j = 0 ; j<size2 ; ++j)
		{
			ptr[j]=bin[i*(sizeof(Type))+j+1];
		}
		data.push_back(tmp);
	}

/* simpler version that uses one byte more.

	unsigned char size = bin[0];
	unsigned char size2= bin[1];
	assert(size2 == sizeof(Type) );
	static Type tmp;
	for(unsigned char i = 0 ; i < size ; ++i )
	{
		unsigned char* ptr = reinterpret_cast<unsigned char*>(&tmp);
		for(int j = 0 ; j<size2 ; ++j)
		{
			ptr[j]=bin[i*(sizeof(Type))+j+2];
		}
		data.push_back(tmp);
	}
*/
}

template<typename Type >
inline void data_to_binary(vector<Type>& data, vector<unsigned char>& bin)
{ 
	unsigned char size3,size = data.size() - 1; 	// 8 bits
	assert(size < 16);
	size3 = size << 4; 				// max size is 16, so 4 bits is enough to store a number = 16-1 = 15
	unsigned char size2 = sizeof(Type) - 1;
	assert(size2 < 16);
	size3 |= size2;  				// 4 bits for nuber of bytes occupied by Type is enough too. (16 bytes max (16-1=15=1+2+4+8))
	bin.push_back(size3);
	for(unsigned char i = 0 ; i <= size ; ++i )
		for(unsigned char j = 0 ; j <= size2 ; ++j)
			bin.push_back( reinterpret_cast<unsigned char*>(&(data[i]))[j] );	 

/* simpler version that uses one byte more

	unsigned char size = data.size();
	unsigned char size2 = sizeof(Type);
	bin.push_back(size);
	bin.push_back(size2);
	for(unsigned char i = 0 ; i < size ; ++i )
		for(unsigned char j = 0 ; j < size2 ; ++j)
			bin.push_back( (reinterpret_cast<unsigned char*>(&(data[i])))[j] );	 
*/
}

template< typename RealType>
struct FundamentalHandler< Vector2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
		{
			const string * tmpStr = any_cast<const string*>(a);
			Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
		
			vector<string> tokens;
			IOFormatManager::parseFundamental(*tmpStr, tokens);
		
			tmp->x() = lexical_cast<RealType>(tokens[0]);
			tmp->y() = lexical_cast<RealType>(tokens[1]);
		}
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			tmp->x() = content[0];
			tmp->y() = content[1];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
		{
			string * tmpStr = any_cast<string*>(a);
			Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
			*tmpStr =	IOFormatManager::getCustomFundamentalOpeningBracket()	+
					lexical_cast<string>(tmp->x())			+
					IOFormatManager::getCustomFundamentalSeparator()	+
					lexical_cast<string>(tmp->y())			+
					IOFormatManager::getCustomFundamentalClosingBracket();
		}
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Vector2<RealType> * tmp = any_cast<Vector2<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back(tmp->x());
			content.push_back(tmp->y());
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Vector3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
		{
			const string * tmpStr = any_cast<const string*>(a);
			Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
			
			vector<string> tokens;
			IOFormatManager::parseFundamental(*tmpStr, tokens);
		
			tmp->x() = lexical_cast<RealType>(tokens[0]);
			tmp->y() = lexical_cast<RealType>(tokens[1]);
			tmp->z() = lexical_cast<RealType>(tokens[2]);
		}
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			tmp->x() = content[0];
			tmp->y() = content[1];
			tmp->z() = content[2];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing to string from some Type
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from string to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Vector3<RealType> * tmp = any_cast<Vector3<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back(tmp->x());
			content.push_back(tmp->y());
			content.push_back(tmp->z());
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Vector4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
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
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			tmp->w() = content[0];
			tmp->x() = content[1];
			tmp->y() = content[2];
			tmp->z() = content[3];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Vector4<RealType> * tmp = any_cast<Vector4<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back(tmp->w());
			content.push_back(tmp->x());
			content.push_back(tmp->y());
			content.push_back(tmp->z());
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix2<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
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
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			(*tmp)(0,0) = content[0];
			(*tmp)(0,1) = content[1];
			(*tmp)(1,0) = content[2];
			(*tmp)(1,1) = content[3];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Matrix2<RealType> * tmp = any_cast<Matrix2<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back((*tmp)(0,0));
			content.push_back((*tmp)(0,1));
			content.push_back((*tmp)(1,0));
			content.push_back((*tmp)(1,1));
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
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
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Matrix3<RealType> * tmp = any_cast<Matrix3<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			(*tmp)(0,0) = content[0];
			(*tmp)(0,1) = content[1];
			(*tmp)(0,2) = content[2];
			(*tmp)(1,0) = content[3];
			(*tmp)(1,1) = content[4];
			(*tmp)(1,2) = content[5];
			(*tmp)(2,0) = content[6];
			(*tmp)(2,1) = content[7];
			(*tmp)(2,2) = content[8];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Matrix3<RealType> * tmp = any_cast<Matrix3<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back((*tmp)(0,0));
			content.push_back((*tmp)(0,1));
			content.push_back((*tmp)(0,2));
			content.push_back((*tmp)(1,0));
			content.push_back((*tmp)(1,1));
			content.push_back((*tmp)(1,2));
			content.push_back((*tmp)(2,0));
			content.push_back((*tmp)(2,1));
			content.push_back((*tmp)(2,2));
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Matrix4<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
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
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Matrix4<RealType> * tmp = any_cast<Matrix4<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			(*tmp)(0,0) = content[0];
			(*tmp)(0,1) = content[1];
			(*tmp)(0,2) = content[2];
			(*tmp)(0,3) = content[3];
			(*tmp)(1,0) = content[4];
			(*tmp)(1,1) = content[5];
			(*tmp)(1,2) = content[6];
			(*tmp)(1,3) = content[7];
			(*tmp)(2,0) = content[8];
			(*tmp)(2,1) = content[9];
			(*tmp)(2,2) = content[10];
			(*tmp)(2,3) = content[11];
			(*tmp)(3,0) = content[12];
			(*tmp)(3,1) = content[13];
			(*tmp)(3,2) = content[14];
			(*tmp)(3,3) = content[15];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Matrix4<RealType> * tmp = any_cast<Matrix4<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back((*tmp)(0,0));
			content.push_back((*tmp)(0,1));
			content.push_back((*tmp)(0,2));
			content.push_back((*tmp)(0,3));
			content.push_back((*tmp)(1,0));
			content.push_back((*tmp)(1,1));
			content.push_back((*tmp)(1,2));
			content.push_back((*tmp)(1,3));
			content.push_back((*tmp)(2,0));
			content.push_back((*tmp)(2,1));
			content.push_back((*tmp)(2,2));
			content.push_back((*tmp)(2,3));
			content.push_back((*tmp)(3,0));
			content.push_back((*tmp)(3,1));
			content.push_back((*tmp)(3,2));
			content.push_back((*tmp)(3,3));
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Quaternion<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
		{
			const string * tmpStr = any_cast<const string*>(a);
			Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
		
			vector<string> tokens;
			IOFormatManager::parseFundamental(*tmpStr, tokens);
		
			RealType angle;
			Vector3<RealType> axis;
		
			if (tokens.size()==3) // Quaternion is written as axis which norm is the angle in radian
			{
				axis[0] = lexical_cast<RealType>(tokens[0]);
				axis[1] = lexical_cast<RealType>(tokens[1]);
				axis[2] = lexical_cast<RealType>(tokens[2]);
				angle = axis.normalize();
			}
			else // tokens.size()==4 Quaternion is written as axis angle
			{
				axis[0] = lexical_cast<RealType>(tokens[0]);
				axis[1] = lexical_cast<RealType>(tokens[1]);
				axis[2] = lexical_cast<RealType>(tokens[2]);
				angle   = lexical_cast<RealType>(tokens[3]);
			}
			tmp->fromAxisAngle(axis,angle);
		}
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			tmp->w() = content[0];
			tmp->x() = content[1];
			tmp->y() = content[2];
			tmp->z() = content[3];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Quaternion<RealType> * tmp = any_cast<Quaternion<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back(tmp->w());
			content.push_back(tmp->x());
			content.push_back(tmp->y());
			content.push_back(tmp->z());
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};


template< typename RealType>
struct FundamentalHandler< Se3<RealType> >
{
	static void creator(Archive& ac, any& a)
	{
		if (a.type()==typeid(const string*)) // deserialization - reading from string to some Type
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
		
			if (tokens.size()==6) // Quaternion is written as axis which norm is the angle in radian
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
		else if (a.type()==typeid(const vector<unsigned char>*)) // from binary stream to Type
		{
			const vector<unsigned char>* tmpBin = any_cast< const vector<unsigned char>* >(a);
			Se3<RealType> * tmp = any_cast<Se3<RealType>*>(ac.getAddress());
			static vector<RealType> content;
			content.clear();
			binary_to_data(*tmpBin,content);
			tmp->position.x() = content[0];
			tmp->position.y() = content[1];
			tmp->position.z() = content[2];
			tmp->orientation.w() = content[3];
			tmp->orientation.x() = content[4];
			tmp->orientation.y() = content[5];
			tmp->orientation.z() = content[6];
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError);
	}
	
	static void accessor(Archive& ac, any& a)
	{
		if (a.type()==typeid(string*)) // serialization - writing from Type to string
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
		else if (a.type()==typeid(vector<unsigned char>*)) // from Vector2<RealType> to binary stream
		{
			vector<unsigned char>* tmpBin = any_cast< vector<unsigned char>* >(a);
			Se3<RealType> * tmp = any_cast<Se3<RealType>*>(ac.getAddress());
			(*tmpBin).clear();
			static vector<RealType> content;
			content.clear();
			content.push_back(tmp->position.x());
			content.push_back(tmp->position.y());
			content.push_back(tmp->position.z());
			content.push_back(tmp->orientation.w());
			content.push_back(tmp->orientation.x());
			content.push_back(tmp->orientation.y());
			content.push_back(tmp->orientation.z());
			data_to_binary(content,*tmpBin);
		}
		else
			throw HandlerError(SerializationExceptions::ExtraCopyError); 
	}
};

#endif //  KNOWNFUNDAMENTALSHANDLER_HPP

