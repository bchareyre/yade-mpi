/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/IOFormatManager.hpp>
#include"XMLSaxParser.hpp"

class XMLFormatManager : public IOFormatManager
{
	private :
		static XmlSaxParser saxParser;

	public :
		XMLFormatManager();
		virtual ~XMLFormatManager();

		virtual void beginSerialization(ostream& stream, Archive& ac);
		virtual void finalizeSerialization(ostream& stream, Archive& ac);
		virtual string beginDeserialization(istream& stream, Archive& ac);
		virtual void finalizeDeserialization(istream& stream, Archive& ac);

		static void serializeSerializable(ostream& stream, Archive& ac, int depth);
		static void serializeContainer(ostream& stream, Archive& ac , int depth);
		static void serializeSmartPointer(ostream& stream, Archive& ac , int depth);

		static void deserializeSerializable(istream& stream, Archive& ac, const string& str="");
		static void deserializeContainer(istream& stream, Archive& ac, const string& str="");
		static void deserializeSmartPointer(istream& stream, Archive& ac, const string& str="");
		static void deserializeFundamental(istream& stream, Archive& ac, const string& str);

		static void writeTabs(ostream& stream, int depth);
		static void writeOpeningTag(ostream& stream, Archive& ac, int depth);
		static void writeClosingTag(ostream& stream, Archive& ac, int depth);

	REGISTER_CLASS_NAME(XMLFormatManager);
	REGISTER_BASE_CLASS_NAME(IOFormatManager);
};

REGISTER_FACTORABLE(XMLFormatManager);


