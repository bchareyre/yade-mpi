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

#ifndef __QTCODEGENERATOR_HPP__
#define __QTCODEGENERATOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGeneratedCodeGenerator.h>
#include <yade-lib-factory/Factorable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief 

	
*/
class QtCodeGenerator : public QtGeneratedCodeGenerator, public Factorable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : string baseDataClass;
	private : string baseEngineClass;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*! Constructor */
	public : QtCodeGenerator (QWidget * parent = 0, const char * name = 0 );

	/*! Destructor */
	public : virtual ~QtCodeGenerator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : void writeHeader(ofstream& s, const string& name, const string& email);
	private : void writeSperarationLines(ofstream& s);
	private : void writeIfDef(ofstream& s,bool start);
	private : void writeIfDef(ofstream& s, const string& name, bool start);
	private : bool testDirectory(const string& dirName,const string& className);
	private : void writeAttributesArea(ofstream& s);
	private : void writeMethodsArea(ofstream& s);
	private : void writeConstructorDestructorArea(ofstream& s);
	private : void writeSerializationArea(ofstream& s);
	private : void writePutYourCodeHereArea(ofstream& s, int nbTabs=0);
	
 	public slots : virtual void bgDataTypeClicked(int);
 	public slots : virtual void pbDataPathClicked();
	public slots : virtual void pbGenerateDataClassClicked();
	public slots : virtual void pbLoadDataClassClicked();
	public slots : virtual void pbSaveDataClassClicked();

	public slots : virtual void bgEngineTypeClicked(int);
	public slots : virtual void pbEnginePathClicked();
	public slots : virtual void pbGenerateEngineClassClicked();
	public slots : virtual void pbAddEngineUnitParamClicked();
	public slots : virtual void pbAddMetaEngineParamClicked();
	public slots : virtual void pbSaveEngineClassClicked();
	public slots : virtual void pbLoadEngineClassClicked();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(QtCodeGenerator);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTCODEGENERATOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

