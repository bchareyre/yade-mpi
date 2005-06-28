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

#include "QtCodeGenerator.hpp"
#include "FileDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/Omega.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistbox.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>
#include <iterator>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtCodeGenerator::QtCodeGenerator ( QWidget * parent , const char * name) : QtGeneratedCodeGenerator(parent,name)
{
	bgDataTypeClicked(0); // to init the default data class to be geometricalModel
	bgEngineTypeClicked(0); // to init the default data class to be geometricalModel
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtCodeGenerator::~QtCodeGenerator ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Data class generation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::bgDataTypeClicked(int v)
{
	switch (v)
	{
		case 0 : baseDataClass = "GeometricalModel"; break;
		case 1 : baseDataClass = "InteractingGeometry"; break;
		case 2 : baseDataClass = "BoundingVolume"; break;
		case 3 : baseDataClass = "PhysicalParameters"; break;
		case 4 : baseDataClass = "State"; break;
		case 5 : baseDataClass = "InteractionGeometry"; break;
		case 6 : baseDataClass = "InteractionPhysics"; break;
		case 7 : baseDataClass = "PhysicalAction"; break;
	}

	cbDataInheritsFrom->clear();
	cbDataInheritsFrom->insertItem(baseDataClass);
	map<string,DynlibType>::const_iterator di    = Omega::instance().getDynlibsType().begin();
	map<string,DynlibType>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
	for(;di!=diEnd;++di)
	{
		if ((*di).second.baseClass==baseDataClass)
			cbDataInheritsFrom->insertItem((*di).first);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbDataPathClicked()
{
	leDataPath->setText(FileDialog::getExistingDirectory ( "./","Choose Path For your Data Class Plugin",this->parentWidget()->parentWidget()).c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbGenerateDataClassClicked()
{
 	bool generateDestructor = cbDataGenerateDestructor->isChecked();
	bool generateRegisterAttributes = cbDataGenerateRegisterAttributes->isChecked();
 	bool generatePostProcessAttributes = cbDataGeneratePostProcessAttributes->isChecked();
	bool generatePreProcessAttributes = cbDataGeneratePreProcessAttributes->isChecked();

	string name = leDataYourName->text();
	string email = leDataYourEmail->text();
	string directory = leDataPath->text().data();

	if (directory[directory.size()-1]!='/') directory.push_back('/');

	string className = leDataClassName->text().data();
	string inheritsFrom = cbDataInheritsFrom->currentText().data();
	string hppFile = directory+className+string("/")+className+string(".hpp");
	string cppFile = directory+className+string("/")+className+string(".cpp");

	if (testDirectory(directory,className))
	{
		ofstream hpp(hppFile.c_str());
		ofstream cpp(cppFile.c_str());

///
/// Generating hpp file
///
		writeHeader(hpp,name,email);
		
		writeSperarationLines(hpp);
		
		writeIfDef(hpp,className,true);
		
		writeSperarationLines(hpp);
		
		hpp << "#include <" << inheritsFrom << ".hpp>" << endl;
		
		writeSperarationLines(hpp);
	
		hpp << "class " << className << " : public " << inheritsFrom << endl;
		hpp << "{" << endl;

		writeAttributesArea(hpp);
		writeMethodsArea(hpp);
		
		if (generateDestructor)
			writeConstructorDestructorArea(hpp);
		
		hpp << "\t public    : " << className << "();" << endl;
		
		if (generateDestructor)
			hpp << "\t public    : virtual ~" << className << "();" << endl;

		writeSerializationArea(hpp);
		if (generateRegisterAttributes)
			hpp << "\t public    : virtual void registerAttributes();" << endl;
	 	if (generatePostProcessAttributes)
			hpp << "\t protected : virtual void postProcessAttributes(bool deserializing);" << endl;
		if (generatePreProcessAttributes)
			hpp << "\t protected : virtual void preProcessAttributes(bool deserializing);" << endl;
		
		if (generateRegisterAttributes||generatePostProcessAttributes||generatePreProcessAttributes)
 			hpp << endl;

		hpp << "\t REGISTER_CLASS_NAME(" << className << ");" << endl;

		hpp << "}" << endl;
		
		writeSperarationLines(hpp);
	
		hpp << "REGISTER_SERIALIZABLE(" << className << ",false);" << endl;

		writeSperarationLines(hpp);

		writeIfDef(hpp,className,false);

		writeSperarationLines(hpp);

///
/// Generating cpp file
///
		writeHeader(cpp,name,email);
		
		writeSperarationLines(cpp);

		cpp << "#include \"" << className << ".hpp\"" << endl;

		writeSperarationLines(cpp);

		cpp << className << "::" << className << "() : " << inheritsFrom << "()" << endl;
		cpp << "{" << endl;
		cpp << "\t createIndex();" << endl;
		writePutYourCodeHereArea(cpp,1);
		cpp << "}" << endl;
		
		writeSperarationLines(cpp);
	
		if (generateDestructor)
		{
			cpp << className << "::~" << className << "()" << endl;
			cpp << "{" << endl;
			writePutYourCodeHereArea(cpp);
			cpp << "}" << endl;
		}

		writeSperarationLines(cpp);
	
		if (generateRegisterAttributes)
		{
			cpp << "void " << className <<"::registerAttributes()" << endl;
			cpp << "{" << endl;
			cpp << "\t" << inheritsFrom << "::registerAttributes();" << endl;
			writePutYourCodeHereArea(cpp,1);
			cpp << "}" << endl;
		}		

		writeSperarationLines(cpp);

	 	if (generatePostProcessAttributes)
		{
			cpp << "void " << className <<"::postProcessAttributes(bool deserializing)" << endl;
			cpp << "{" << endl;
			cpp << "\t" << inheritsFrom << "::postProcessAttributes(deserializing);" << endl;
			writePutYourCodeHereArea(cpp,1);
			cpp << "}" << endl;
		}

		writeSperarationLines(cpp);

		if (generatePreProcessAttributes)
		{
			cpp << "void " << className <<"::preProcessAttributes(bool deserializing)" << endl;
			cpp << "{" << endl;
			cpp << "\t" << inheritsFrom << "::preProcessAttributes();" << endl;
			writePutYourCodeHereArea(cpp,1);
			cpp << "}" << endl;
		}

		writeSperarationLines(cpp);

		hpp.close();
		cpp.close();
	}
	else
	{
	}
	

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbLoadDataClassClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbSaveDataClassClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Engine generation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::bgEngineTypeClicked(int v )
{
	switch (v)
	{
		case 0 : baseEngineClass = "Engine"; break;
		case 1 : baseEngineClass = "EngineUnit"; break;
		case 2 : baseEngineClass = "MetaDispatchingEngine1D"; break;
		case 3 : baseEngineClass = "MetaDispatchingEngine2D"; break;
	}

	if (baseEngineClass=="Engine")
	{
		gbMetaDispatchingEngineProperties->setEnabled(false);
		gbEngineUnitProperties->setEnabled(false);
	}
	else if (baseEngineClass=="EngineUnit")
	{
		gbMetaDispatchingEngineProperties->setEnabled(false);
		gbEngineUnitProperties->setEnabled(true);
	}
	else if (baseEngineClass=="MetaDispatchingEngine1D")
	{
		gbMetaDispatchingEngineProperties->setEnabled(true);
		gbEngineUnitProperties->setEnabled(false);
		tlMetaEngineBaseClass2->setEnabled(false);
		cbMetaEngineBaseClass2->setEnabled(false);
	}
	else if (baseEngineClass=="MetaDispatchingEngine2D")
	{
		gbMetaDispatchingEngineProperties->setEnabled(true);
		gbEngineUnitProperties->setEnabled(false);
		tlMetaEngineBaseClass2->setEnabled(true);
		cbMetaEngineBaseClass2->setEnabled(true);
	}
	
	if (baseEngineClass=="MetaDispatchingEngine1D")
	{
		cbMetaEngineBaseClass1->clear();
		map<string,DynlibType>::const_iterator di    = Omega::instance().getDynlibsType().begin();
		map<string,DynlibType>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
		for(;di!=diEnd;++di)
		{
			if ((*di).second.isIndexable)
				cbMetaEngineBaseClass1->insertItem((*di).first);
		}
	}

	if (baseEngineClass=="MetaDispatchingEngine2D")
	{
		cbMetaEngineBaseClass2->clear();
		map<string,DynlibType>::const_iterator di    = Omega::instance().getDynlibsType().begin();
		map<string,DynlibType>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
		for(;di!=diEnd;++di)
		{
			if ((*di).second.isIndexable)
				cbMetaEngineBaseClass2->insertItem((*di).first);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbEnginePathClicked()
{
	leEnginePath->setText(FileDialog::getExistingDirectory ( "./","Choose Path For your Engine Plugin",this->parentWidget()->parentWidget()).c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbAddEngineUnitParamClicked()
{
	lbEngineUnitParam->insertItem(leEngineUnitParam->text());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbAddMetaEngineParamClicked()
{
	lbMetaEngineParam->insertItem(leMetaEngineParam->text());
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbGenerateEngineClassClicked()
{
	bool generateConstructor = cbEngineGenerateConstructor->isChecked();
	bool generateDestructor = cbEngineGenerateDestructor->isChecked();
	bool generateRegisterAttributes = cbEngineGenerateRegisterAttributes->isChecked();
 	bool generatePostProcessAttributes = cbEngineGeneratePostProcessAttributes->isChecked();
	bool generatePreProcessAttributes = cbEngineGeneratePreProcessAttributes->isChecked();

	string name = leEngineYourName->text();
	string email = leEngineYourEmail->text();
	string directory = leEnginePath->text().data();

	if (directory[directory.size()-1]!='/') directory.push_back('/');

	string className = leEngineClassName->text().data();

	string metaEngineBaseClass1,metaEngineBaseClass2;
	string engineUnitBaseClass1,engineUnitBaseClass2;
	string metaEngineEngineUnitName,engineUnitBaseClass;
	string metaEngineEngineUnitReturnType, engineUnitReturnType;

	if (baseEngineClass=="EngineUnit")
	{
		//engineUnitBaseClass1
		//engineUnitBaseClass1
		engineUnitBaseClass	= leEngineUnitBaseClass->text().data();
		engineUnitReturnType	= leEngineUnitReturnType->text().data();
	}
	else if (baseEngineClass=="MetaDispatchingEngine1D" || baseEngineClass=="MetaDispatchingEngine2D")
	{
		metaEngineBaseClass1		= cbMetaEngineBaseClass1->currentText().data();
		metaEngineEngineUnitName	= leMetaEngineEngineUnitName->text().data();
		metaEngineEngineUnitReturnType	= leMetaEngineEngineUnitReturnType->text().data();
 		if (baseEngineClass=="MetaDispatchingEngine2D")
			metaEngineBaseClass2	= cbMetaEngineBaseClass2->currentText().data();
	}

	string hppFile = directory+className+string("/")+className+string(".hpp");
	string cppFile = directory+className+string("/")+className+string(".cpp");

	if (testDirectory(directory,className))
	{
		ofstream hpp(hppFile.c_str());
		ofstream cpp(cppFile.c_str());

///
/// Generating hpp file
///
		writeHeader(hpp,name,email);
		
		writeSperarationLines(hpp);
		
		writeIfDef(hpp,className,true);
		
		writeSperarationLines(hpp);
		
		if (baseEngineClass=="Engine" || baseEngineClass=="MetaDispatchingEngine1D" || baseEngineClass=="MetaDispatchingEngine2D")	
			hpp << "#include <Engine.hpp>" << endl;
		if (baseEngineClass=="EngineUnit")	
			hpp << "#include <" << engineUnitBaseClass << ".hpp>" << endl;
		
		writeSperarationLines(hpp);
	
		if (baseEngineClass=="Engine")
			hpp << "class " << className << " : public Engine" << endl;
		else if (baseEngineClass=="MetaDispatchingEngine1D" || baseEngineClass=="MetaDispatchingEngine2D" )
		{
			hpp << "class " << className << " : " << endl;
			hpp << "		public Engine, " << endl;
			hpp << "		public DynLibDispatcher" << endl;
			hpp << "		<"<<endl;
			if (baseEngineClass=="MetaDispatchingEngine1D")
				hpp << "			TYPELIST_1(" << metaEngineBaseClass1 << ")," << endl;
			else
				hpp << "			TYPELIST_2(" << metaEngineBaseClass1 << ", " << metaEngineBaseClass1 << ")," << endl;
			hpp << "			" << metaEngineEngineUnitName << "," << endl;
			hpp << "			" << metaEngineEngineUnitReturnType << "," << endl;
		
			/*if (lbMetaEngineParam->count()==0)
			{
				if (baseEngineClass=="MetaDispatchingEngine1D")
					hpp << "			TYPELIST_1(const shared_ptr<"<<metaEngineBaseClass1<<">&)" << endl;
				else
					hpp << "			TYPELIST_2(const shared_ptr<"<<metaEngineBaseClass1<< ">&), " << metaEngineBaseClass2 << ">&)" << endl;
			}
			else
			{*/
				int nb = lbMetaEngineParam->count();
				string tab;
				if (nb>0 || baseEngineClass=="MetaDispatchingEngine2D") tab="\t";
				if (baseEngineClass=="MetaDispatchingEngine1D")
					hpp << "			TYPELIST_"<<nb+1<<"("<<tab<<"const shared_ptr<"<<metaEngineBaseClass1<<">& " ;					
				else
				{
					hpp << "			TYPELIST_"<<nb+2<<"("<<tab<<"const shared_ptr<"<<metaEngineBaseClass1<<">&, " << endl;
					hpp << "					const shared_ptr<"<<metaEngineBaseClass2<<">& ";
				}

				if (nb==0)
					hpp << ")" << endl;
				else
					hpp << "," << endl;

				for(int i=0;i<nb;i++)
				{
					string currentParam = lbMetaEngineParam->text(i).data();
					if (i!=nb-1)
						hpp << "					" << currentParam << " ," << endl;
					else
					{
						hpp << "					" << currentParam << endl;
						hpp << "			)" << endl;				
					}
				}			
			//}
			hpp << "		>" << endl;
		}
		else if(baseEngineClass=="EngineUnit")
		{

		}

		hpp << "{" << endl;

		writeAttributesArea(hpp);
		writeMethodsArea(hpp);
		
		if (generateConstructor || generateDestructor)
			writeConstructorDestructorArea(hpp);
		
		if (generateConstructor)
			hpp << "\t public    : " << className << "();" << endl;
		
		if (generateDestructor)
			hpp << "\t public    : virtual ~" << className << "();" << endl;

		writeSerializationArea(hpp);
		if (generateRegisterAttributes)
			hpp << "\t public    : virtual void registerAttributes();" << endl;
	 	if (generatePostProcessAttributes)
			hpp << "\t protected : virtual void postProcessAttributes(bool deserializing);" << endl;
		if (generatePreProcessAttributes)
			hpp << "\t protected : virtual void preProcessAttributes(bool deserializing);" << endl;
		
		if (generateRegisterAttributes||generatePostProcessAttributes||generatePreProcessAttributes)
 			hpp << endl;

		hpp << "\t REGISTER_CLASS_NAME(" << className << ");" << endl;

		hpp << "}" << endl;
		
		writeSperarationLines(hpp);
	
		hpp << "REGISTER_SERIALIZABLE(" << className << ",false);" << endl;

		writeSperarationLines(hpp);

		writeIfDef(hpp,className,false);

		writeSperarationLines(hpp);

///
/// Generating cpp file
///
// 		writeHeader(cpp,name,email);
// 		
// 		writeSperarationLines(cpp);
// 
// 		cpp << "#include \"" << className << ".hpp\"" << endl;
// 
// 		writeSperarationLines(cpp);
// 
// 		cpp << className << "::" << className << "() : " << inheritsFrom << "()" << endl;
// 		cpp << "{" << endl;
// 		cpp << "\t createIndex();" << endl;
// 		writePutYourCodeHereArea(cpp,1);
// 		cpp << "}" << endl;
// 		
// 		writeSperarationLines(cpp);
// 	
// 		if (generateDestructor)
// 		{
// 			cpp << className << "::~" << className << "()" << endl;
// 			cpp << "{" << endl;
// 			writePutYourCodeHereArea(cpp);
// 			cpp << "}" << endl;
// 		}
// 
// 		writeSperarationLines(cpp);
// 	
// 		if (generateRegisterAttributes)
// 		{
// 			cpp << "void " << className <<"::registerAttributes()" << endl;
// 			cpp << "{" << endl;
// 			cpp << "\t" << inheritsFrom << "::registerAttributes();" << endl;
// 			writePutYourCodeHereArea(cpp,1);
// 			cpp << "}" << endl;
// 		}		
// 
// 		writeSperarationLines(cpp);
// 
// 	 	if (generatePostProcessAttributes)
// 		{
// 			cpp << "void " << className <<"::postProcessAttributes(bool deserializing)" << endl;
// 			cpp << "{" << endl;
// 			cpp << "\t" << inheritsFrom << "::postProcessAttributes(deserializing);" << endl;
// 			writePutYourCodeHereArea(cpp,1);
// 			cpp << "}" << endl;
// 		}
// 
// 		writeSperarationLines(cpp);
// 
// 		if (generatePreProcessAttributes)
// 		{
// 			cpp << "void " << className <<"::preProcessAttributes(bool deserializing)" << endl;
// 			cpp << "{" << endl;
// 			cpp << "\t" << inheritsFrom << "::preProcessAttributes();" << endl;
// 			writePutYourCodeHereArea(cpp,1);
// 			cpp << "}" << endl;
// 		}
// 
// 		writeSperarationLines(cpp);

		hpp.close();
		cpp.close();
	}
	else
	{
	}
	

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbSaveEngineClassClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbLoadEngineClassClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Common function
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeHeader(ofstream& s, const string& name, const string& email)
{
	string space1,space2;
	for(int i=0;i<=48-name.size();i++)
		space1.push_back(' ');
	for(int i=0;i<=70-email.size();i++)
		space2.push_back(' ');

	s << "/***************************************************************************" << endl;
	s << "*   Copyright (C) 2004 by " << name << space1 <<                          "*" << endl;
	s << "*   " << email << space2 <<                                               "*" << endl;
	s << "*                                                                          *" << endl;
	s << "*   This program is free software; you can redistribute it and/or modify   *" << endl;
	s << "*   it under the terms of the GNU General Public License as published by   *" << endl;
	s << "*   the Free Software Foundation; either version 2 of the License, or      *" << endl;
	s << "*   (at your option) any later version.                                    *" << endl;
	s << "*                                                                          *" << endl;
	s << "*   This program is distributed in the hope that it will be useful,        *" << endl;
	s << "*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *" << endl;
	s << "*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *" << endl;
	s << "*   GNU General Public License for more details.                           *" << endl;
	s << "*                                                                          *" << endl;
	s << "*   You should have received a copy of the GNU General Public License      *" << endl;
	s << "*   along with this program; if not, write to the                          *" << endl;
	s << "*   Free Software Foundation, Inc.,                                        *" << endl;
	s << "*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *" << endl;
	s << "***************************************************************************/" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
void QtCodeGenerator::writeSperarationLines(ofstream& s)
{
	s << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeAttributesArea(ofstream& s)
{
	s << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << "/// ATTRIBUTES                                                                                   //" << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	writePutYourCodeHereArea(s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeMethodsArea(ofstream& s)
{
	s << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << "/// METHODS                                                                                      //" << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	writePutYourCodeHereArea(s);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeConstructorDestructorArea(ofstream& s)
{
	s << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << "/// CONSTRUCTOR/DESTRUCTOR                                                                       //" << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeSerializationArea(ofstream& s)
{
	s << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << "/// SERIALIZATION                                                                                //" << endl;
	s << "///////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
	s << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writePutYourCodeHereArea(ofstream& s, int nbTabs)
{
	s << endl;
	
	for(int i=0;i<nbTabs;i++)
		s << "\t";
	s << "///" << endl;
	for(int i=0;i<nbTabs;i++)
		s << "\t";
	s << "/// PUT YOUR CODE HERE" << endl;
	for(int i=0;i<nbTabs;i++)
		s << "\t";
	s << "///" << endl;
	s << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::writeIfDef(ofstream& s, const string& name, bool start)
{
	string upperCaseName;
	upperCaseName.resize(name.size());
	transform (name.begin(),name.end(), upperCaseName.begin(), (int(*)(int))toupper);
		
	if (start)
	{
		s << "#ifndef __"+upperCaseName+"_HPP__" << endl;
		s << "#define __"+upperCaseName+"_HPP__" << endl;
	}
	else
	{
		s << "#endif // __"+upperCaseName+"_HPP__" << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool QtCodeGenerator::testDirectory(const string& dirName,const string& className)
{
	
	filesystem::path path = filesystem::path(dirName, filesystem::native);

	if ( filesystem::exists( path ) )
	{
		filesystem::create_directories(dirName+className);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
