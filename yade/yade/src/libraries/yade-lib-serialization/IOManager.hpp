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

#ifndef __IOMANAGER__
#define __IOMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-factory/Factorable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Serializable;
class Archive;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class IOManager : public Factorable
{
	public   : IOManager();
	public   : virtual ~IOManager();

	public  : virtual void beginSerialization(ostream& ,  Archive& )     {};
	public  : virtual void finalizeSerialization(ostream& ,  Archive& )  {};
	public  : virtual string beginDeserialization(istream& ,  Archive& ) { return "";};
	public  : virtual void finalizeDeserialization(istream& , Archive& ) {};
	
	private    : static char cOB;	// containerOpeningBracket 
	private    : static char cCB;	// containerClosingBracket
	private    : static char cS;	// containerSeparator
	private    : static char cfOB;	// customFundamentalOpeningBracket
	private    : static char cfCB;	// customFundamentalClosingBracket
	private    : static char cfS; 	// customFundamentalSeparator
	
	protected  : void setContainerOpeningBracket(char c) {cOB=c;};
	protected  : void setContainerClosingBracket(char c) {cCB=c;};
	protected  : void setContainerSeparator(char c)      {cS=c;};
	
	protected  : void setCustomFundamentalOpeningBracket(char c) {cfOB=c;};
	protected  : void setCustomFundamentalClosingBracket(char c) {cfCB=c;};
	protected  : void setCustomFundamentalSeparator(char c)      {cfS=c;};
	
	public  : static char getContainerOpeningBracket() {return cOB;};
	public  : static char getContainerClosingBracket() {return cCB;};
	public  : static char getContainerSeparator()      {return cS;};
	 
	public  : static char getCustomFundamentalOpeningBracket() {return cfOB;};
	public  : static char getCustomFundamentalClosingBracket() {return cfCB;};
	public  : static char getCustomFundamentalSeparator()      {return cfS;};
	
	public : static void parseFundamental(const string& str, vector<string>& tokens);
	
	public : static void serializeFundamental(ostream& stream, Archive& ac, int depth);
	public : static void deserializeFundamental(istream& stream, Archive& ac, const string& str);

	public : static void serializeCustomClass(ostream& stream, Archive& ac , int depth);
	public : static void deserializeCustomClass(istream& stream, Archive& ac, const string& str="");

	public : static void serializeSmartPointerOfFundamental(ostream& stream, Archive& ac , int depth);
	public : static void deserializeSmartPointerOfFundamental(istream& stream, Archive& ac, const string& str);
	
	public    : static void serializeCustomFundamental(ostream& stream, Archive& ac, int depth);
	public    : static void serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth);
	public    : static void serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth);

	public    : static void deserializeCustomFundamental(istream& stream, Archive& ac,const string& str);
	public    : static void deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str);
	public    : static void deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str);

	public : template<typename Type>
		 static void loadFromFile(const string& libName, const string& fileName,const string& name, Type& t);

	public : template<typename Type>
		 static void saveToFile(const string& libName, const string& fileName,const string& name, Type& t);


	public : template<typename Type>
		 void loadArchive(istream& stream, Type& t, const string& name);

	public : template<typename Type>
		 void saveArchive(ostream& stream, Type& t, const string& name);

};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "IOManager.tpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(IOManager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __IOMANAGER__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
