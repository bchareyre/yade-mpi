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

#include "QtFileGenerator.hpp"
#include "ClassFactory.hpp"
#include "FileGenerator.hpp"
#include "Omega.hpp"
#include "FileDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtFileGenerator::QtFileGenerator ( QWidget * parent , const char * name) : QtFileGeneratorController(parent,name)
{
	setMinimumSize(size());
	setMaximumSize(size());	
	
	map<string,string>::const_iterator di    = Omega::instance().getDynlibsType().begin();
	map<string,string>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
	for(;di!=diEnd;++di)
	{
		if ((*di).second=="IOManager")
			cbSerializationName->insertItem((*di).first);
		else if ((*di).second=="FileGenerator")
			cbGeneratorName->insertItem((*di).first);
	}

	cbGeneratorNameActivated(cbGeneratorName->currentText());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtFileGenerator::~QtFileGenerator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtFileGenerator::pbChooseClicked()
{
	string selectedFilter;
	string fileName = FileDialog::getSaveFileName("../data", "XML Yade File (*.xml)", "Choose a file to save", this->parentWidget()->parentWidget(),selectedFilter );

	if (fileName.size()!=0 && selectedFilter == "XML Yade File (*.xml)")
		leOutputFileName->setText(fileName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtFileGenerator::cbGeneratorNameActivated(const QString& s)
{
	try
	{
		//FIXME dynamic_cast is not working ???
		shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(s));

		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(false);
		guiGen.setShift(10,30);
		guiGen.setShowButtons(false);
		
		QSize s = gbGeneratorParameter->size();
		QPoint p = gbGeneratorParameter->pos();
		delete gbGeneratorParameter;
		gbGeneratorParameter = new QGroupBox(this);
		gbGeneratorParameter->resize(s.width(),s.height());
		gbGeneratorParameter->move(p.x(),p.y()); 
		gbGeneratorParameter->setTitle(fg->getClassName()+" Parameters"); 
		
		guiGen.buildGUI(fg,gbGeneratorParameter);
		
		gbGeneratorParameter->show();
		s = gbGeneratorParameter->size();
		p = gbGeneratorParameter->pos();
		setMinimumSize(size().width(),s.height()+p.y()+50);
		setMaximumSize(size().width(),s.height()+p.y()+50);
		resize(size().width(),s.height()+p.y()+50);
		pbGenerate->move(pbGenerate->pos().x(),s.height()+p.y()+10);
		pbClose->move(pbClose->pos().x(),s.height()+p.y()+10);
	}
	catch (FactoryError&)
	{
	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "MessageDialog.hpp"
void QtFileGenerator::pbGenerateClicked()
{
	// FIXME add some test to avoid crashing
	shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(cbGeneratorName->currentText()));
	
	fg->setFileName(leOutputFileName->text());
	fg->setSerializationLibrary(cbSerializationName->currentText());
	
	guiGen.deserialize(fg);
	
	string message = fg->generateAndSave();

	string fileName = string(filesystem::basename(leOutputFileName->text().data()))+string(filesystem::extension(leOutputFileName->text().data()));
	shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("File "+fileName+" generated successfully.\n\n"+message,this->parentWidget()->parentWidget()));
	md->exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtFileGenerator::pbCloseClicked()
{
	close();
	destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtFileGenerator::closeEvent(QCloseEvent *evt)
{
	close();
	QtFileGeneratorController::closeEvent(evt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
