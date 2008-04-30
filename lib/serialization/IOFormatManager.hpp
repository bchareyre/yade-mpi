/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef IOFORMATMANAGER_HPP
#define IOFORMATMANAGER_HPP

#include <iostream>
#include <fstream>
#include<yade/lib-factory/Factorable.hpp>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class Serializable;
class Archive;

using namespace std;
using namespace boost;

class IOFormatManager : public Factorable
{
	public:
		enum {FORMAT_BIN,FORMAT_XML};
		static int format;
	private :
		static char cOB;	// containerOpeningBracket 
		static char cCB;	// containerClosingBracket
		static char cS;		// containerSeparator
		static char cfOB;	// customFundamentalOpeningBracket
		static char cfCB;	// customFundamentalClosingBracket
		static char cfS; 	// customFundamentalSeparator
	
	protected :
		void setContainerOpeningBracket(char c) {cOB=c;};
		void setContainerClosingBracket(char c) {cCB=c;};
		void setContainerSeparator(char c)      {cS=c;};
		
		void setCustomFundamentalOpeningBracket(char c) {cfOB=c;};
		void setCustomFundamentalClosingBracket(char c) {cfCB=c;};
		void setCustomFundamentalSeparator(char c)      {cfS=c;};

	public :
		IOFormatManager();
		virtual ~IOFormatManager();
		
		virtual void beginSerialization(ostream& ,  Archive& );
		virtual void finalizeSerialization(ostream& ,  Archive& );
		virtual string beginDeserialization(istream& ,  Archive& );
		virtual void finalizeDeserialization(istream& , Archive& );
		
		static char getContainerOpeningBracket() {return cOB;};
		static char getContainerClosingBracket() {return cCB;};
		static char getContainerSeparator()      {return cS;};
		
		static char getCustomFundamentalOpeningBracket() {return cfOB;};
		static char getCustomFundamentalClosingBracket() {return cfCB;};
		static char getCustomFundamentalSeparator()      {return cfS;};

		static void parseFundamental(const string& str, vector<string>& tokens);
		
		static void serializeFundamental(ostream& stream, Archive& ac, int depth);
		static void deserializeFundamental(istream& stream, Archive& ac, const string& str);
		
		static void serializeCustomClass(ostream& stream, Archive& ac , int depth);
		static void deserializeCustomClass(istream& stream, Archive& ac, const string& str="");
		
		static void serializeSmartPointerOfFundamental(ostream& stream, Archive& ac , int depth);
		static void deserializeSmartPointerOfFundamental(istream& stream, Archive& ac, const string& str);
	
		static void serializeCustomFundamental(ostream& stream, Archive& ac, int depth);
		static void serializeContainerOfFundamental(ostream& stream, Archive& ac, int depth);
		static void serializeFundamentalSerializable(ostream& stream, Archive& ac, int depth);
		
		static void deserializeCustomFundamental(istream& stream, Archive& ac,const string& str);
		static void deserializeContainerOfFundamental(istream& stream, Archive& ac, const string& str);
		static void deserializeFundamentalSerializable(istream& stream, Archive& ac, const string& str);

		template<typename Type>
		static void loadFromFile(const string& libName, const string& fileName,const string& name, Type& t);
		
		template<typename Type>
		static void saveToFile(const string& libName, const string& fileName,const string& name, Type& t);
		
		
		template<typename Type>
		void loadArchive(const string& libName, istream& stream, Type& t, const string& name);
		
		template<typename Type>
		void saveArchive(const string& libName, ostream& stream, Type& t, const string& name);

	REGISTER_CLASS_NAME(IOFormatManager);
	REGISTER_BASE_CLASS_NAME(Serializable);

};

#include "IOFormatManager.tpp"

REGISTER_FACTORABLE(IOFormatManager);

#endif //  IOFORMATMANAGER_HPP

