/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BINFORMATMANAGER_HPP
#define BINFORMATMANAGER_HPP

#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <vector>
#include <string>

class BINFormatManager : public IOFormatManager
{
	private:
		static std::vector<std::string> names;
		
		static void 		  serializeNameMarker(ostream& stream, std::string name);
		static std::string 	deserializeNameMarker(istream& stream);
	public:
		BINFormatManager();
		virtual ~BINFormatManager();

		virtual void 		beginSerialization(ostream& stream, Archive& ac);
		virtual void 		finalizeSerialization(ostream& stream, Archive& ac);
		virtual string 		beginDeserialization(istream& stream, Archive& ac);
		virtual void 		finalizeDeserialization(istream& stream, Archive& ac);

		static void 		  serializeSerializable(ostream& stream, Archive& ac, int depth);
		static void 		deserializeSerializable(istream& stream, Archive& ac, const string& str="");
		
		static void 		  serializeFundamental(ostream& stream, Archive& ac, int depth);
		static void 		deserializeFundamental(istream& stream, Archive& ac, const string& str);
		
		static void 		  serializeContainer(ostream& stream, Archive& ac , int depth);
		static void 		deserializeContainer(istream& stream, Archive& ac, const string& str="");
		
		static void 		  serializeSmartPointer(ostream& stream, Archive& ac , int depth);
		static void 		deserializeSmartPointer(istream& stream, Archive& ac, const string& str="");
		
		static void 		  serializeUnsupported(ostream&, Archive&, int);
		static void 		deserializeUnsupported(istream&, Archive&,const string&);

	REGISTER_CLASS_NAME(BINFormatManager);
	REGISTER_BASE_CLASS_NAME(IOFormatManager);
};

REGISTER_FACTORABLE(BINFormatManager);

#endif // BINFORMATMANAGER_HPP

