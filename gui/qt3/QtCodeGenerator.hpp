/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTCODEGENERATOR_HPP
#define QTCODEGENERATOR_HPP

#include<yade/core/Omega.hpp>
#include<yade/lib-factory/Factorable.hpp>
#include <string>
#include <fstream>
#include <QtGeneratedCodeGenerator.h>

using namespace std;

class QtCodeGenerator : public QtGeneratedCodeGenerator, public Factorable
{
	private :
		string baseDataClass;
		string baseEngineClass;

		void writeHeader(ofstream& s, const string& name, const string& email);
		void writeSperarationLines(ofstream& s);
		void writeIfDef(ofstream& s,bool start);
		void writeIfDef(ofstream& s, const string& name, bool start);
		bool testDirectory(const string& dirName,const string& className);
		void writeAttributesArea(ofstream& s);
		void writeMethodsArea(ofstream& s);
		void writeConstructorDestructorArea(ofstream& s);
		void writeSerializationArea(ofstream& s);
		void writePutYourCodeHereArea(ofstream& s, int nbTabs=0);

	public :
		QtCodeGenerator (QWidget * parent = 0, const char * name = 0 );
		virtual ~QtCodeGenerator ();

 	public slots :
		virtual void bgDataTypeClicked(int);
 		virtual void pbDataPathClicked();
		virtual void pbGenerateDataClassClicked();
		virtual void pbLoadDataClassClicked();
		virtual void pbSaveDataClassClicked();

		virtual void bgEngineTypeClicked(int);
		virtual void pbEnginePathClicked();
		virtual void pbGenerateEngineClassClicked();
		virtual void pbAddEngineUnitParamClicked();
		virtual void pbAddMetaEngineParamClicked();
		virtual void pbSaveEngineClassClicked();
		virtual void pbLoadEngineClassClicked();

	REGISTER_CLASS_NAME(QtCodeGenerator);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

REGISTER_FACTORABLE(QtCodeGenerator);

#endif // QTCODEGENERATOR_HPP

