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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtCodeGenerator::QtCodeGenerator ( QWidget * parent , const char * name) : QtGeneratedCodeGenerator(parent,name)
{
	bgDataTypeClicked(0); // to init the default data class to be geometricalModel
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtCodeGenerator::~QtCodeGenerator ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::bgDataTypeClicked(int v)
{
	switch (v)
	{
		case 0 : baseClass = "GeometricalModel"; break;
		case 1 : baseClass = "InteractingGeometry"; break;
		case 2 : baseClass = "BoundingVolume"; break;
		case 3 : baseClass = "PhysicalParameters"; break;
		case 4 : baseClass = "State"; break;
		case 5 : baseClass = "InteractionGeometry"; break;
		case 6 : baseClass = "InteractionPhysics"; break;
		case 7 : baseClass = "PhysicalAction"; break;
	}

	cbInheritsFrom->clear();
	cbInheritsFrom->insertItem(baseClass);
	map<string,string>::const_iterator di    = Omega::instance().getDynlibsType().begin();
	map<string,string>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
	for(;di!=diEnd;++di)
	{
		if ((*di).second==baseClass)
			cbInheritsFrom->insertItem((*di).first);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbDataPathClicked()
{
	leDataPath->setText(FileDialog::getExistingDirectory ( "./","Choose Path",this->parentWidget()->parentWidget()).c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtCodeGenerator::pbGenerateDataClassClicked()
{

	bool generateRegisterAttributes = cbGenerateDataRegisterAttributes->isChecked();
 	bool generatePostProcessAttributes = cbGenerateDataPostProcessAttributes->isChecked();
	bool generatePreProcessAttributes = cbGenerateDataPreProcessAttributes->isChecked();

	string name = "Galizzi Olivier";
	string email = "olivier.galizzi@imag.fr";

	string pathstr = leDataPath->text().data();
	filesystem::path path = filesystem::path(pathstr, filesystem::native);

	if ( filesystem::exists( path ) )
	{
		string className = leDataClassName->text().data();
		filesystem::create_directories(pathstr+string("/")+className);
		string hppFile = pathstr+string("/")+className+string("/")+className+string(".hpp");
		ofstream file(hppFile.c_str());
		writeHeader(file,name,email);
		writeSperarationLines(file);

		file << "#include <" << baseClass << ".hpp>" << endl;
		writeSperarationLines(file);
	
		file << "class " << className << " : public " << baseClass << endl;
		file << "{" << endl;

		file << "}" << endl;

	}
	else
	{
	}
	

}

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
