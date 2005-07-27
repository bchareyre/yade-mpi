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

#include "QtEngineEditor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-core/MetaDispatchingEngine.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>

#include "GLEngineEditor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtEngineEditor::QtEngineEditor() : QtGeneratedEngineEditor()
{
	map<string,DynlibDescriptor>::const_iterator di    = Omega::instance().getDynlibsDescriptor().begin();
	map<string,DynlibDescriptor>::const_iterator diEnd = Omega::instance().getDynlibsDescriptor().end();
	for(;di!=diEnd;++di)
	{
		if (Omega::instance().isInheritingFrom((*di).first,"MetaEngine"))
			cbMetaEnginesList->insertItem((*di).first);
		else if (Omega::instance().isInheritingFrom((*di).first,"DeusExMachina"))
			cbDeusExMachinaList->insertItem((*di).first);
		else if (Omega::instance().isInheritingFrom((*di).first,"Engine"))
			cbEnginesList->insertItem((*di).first);

	}

	connect( glEngineEditor, SIGNAL( verifyValidity() ), this, SLOT( verifyValidity() ) );
	connect( glEngineEditor, SIGNAL( engineSelected(int) ), this, SLOT( engineSelected(int) ) );
	connect( glEngineEditor, SIGNAL( deleteEngine(int) ), this, SLOT( deleteEngine(int) ) );

	engineFrame = new QFrame();	
	metaDispatchingEngineFrame = new QtMetaDispatchingEngineProperties();

	verifyValidity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtEngineEditor::~QtEngineEditor()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbAddEngineClicked()
{
	string engineName = cbEnginesList->currentText();
	int id = glEngineEditor->addEngine(engineName);

	EngineDescriptor ed;
	ed.engine = dynamic_pointer_cast<Engine>(ClassFactory::instance().createShared(engineName));
	ed.type = STANDALONEENGINE;
	engines[id] = ed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbAddMetaEngineClicked()
{
	int id;
	EngineDescriptor ed;

	string engineName = cbMetaEnginesList->currentText();
	shared_ptr<MetaDispatchingEngine> mde = dynamic_pointer_cast<MetaDispatchingEngine>(ClassFactory::instance().createShared(engineName));

	if (mde)
	{
		if (mde->getDimension()==1)
		{
			id = glEngineEditor->addMetaDispatchingEngine1D(engineName, mde->getEngineUnitType(), mde->getBaseClassType(0));
			ed.type = METADISPATCHINGENGINE1D;
		}
		else if (mde->getDimension()==2)
		{
			id = glEngineEditor->addMetaDispatchingEngine2D(engineName, mde->getEngineUnitType(), mde->getBaseClassType(0), mde->getBaseClassType(0));
			ed.type = METADISPATCHINGENGINE2D;
		}
	}
	else // it is a meta engine
	{
		id = glEngineEditor->addEngine(engineName);
		ed.type = METAENGINE;
	}

	ed.engine = mde;

	engines[id] = ed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbAddDeusExMachinaClicked()
{
	string engineName = cbDeusExMachinaList->currentText();
	int id = glEngineEditor->addDeusExMachina(engineName);

	EngineDescriptor ed;
	ed.engine = dynamic_pointer_cast<Engine>(ClassFactory::instance().createShared(engineName));
	ed.type = DEUSEXMACHINA;
	engines[id] = ed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbSaveClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbLoadClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbPathClicked()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::verifyValidity()
{
	string message;
	bool enabled;
	if (enabled=glEngineEditor->verify(message))
		tlStatus->setBackgroundColor(QColor(0,255,0));
	else
		tlStatus->setBackgroundColor(QColor(255,0,0));

	tlStatus->setText(message.c_str());

	leFileName->setEnabled(enabled);
	pbPath->setEnabled(enabled);
	pbSave->setEnabled(enabled);
	pbLoad->setEnabled(enabled);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::engineSelected(int i)
{
	if (i>=0  && currentEngine!=engines[i].engine) // otherwise the GUI is already created
	{
		try
		{
			currentEngine = engines[i].engine;
	
			guiGen.setResizeHeight(true);
			guiGen.setResizeWidth(true);
			guiGen.setShift(10,20);
			guiGen.setShowButtons(true);
	
			QWidget * parent   = this->parentWidget()->parentWidget();

			if (engines[i].type==DEUSEXMACHINA || engines[i].type==STANDALONEENGINE)
			{
				delete engineFrame;
				engineFrame = new QFrame(parent);
				engineFrame->setCaption(currentEngine->getClassName());
				guiGen.buildGUI(currentEngine,engineFrame);
				engineFrame->show();
			}
			else if (engines[i].type==METADISPATCHINGENGINE2D || engines[i].type==METADISPATCHINGENGINE1D)
			{	
				delete metaDispatchingEngineFrame;
				shared_ptr<MetaDispatchingEngine> mde = dynamic_pointer_cast<MetaDispatchingEngine>(currentEngine);
				metaDispatchingEngineFrame = new QtMetaDispatchingEngineProperties(mde,parent);
				metaDispatchingEngineFrame->setCaption(currentEngine->getClassName());
				metaDispatchingEngineFrame->show();
			}
		}
		catch (FactoryError&)
		{
		
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::deleteEngine(int i)
{

	engines.erase(i);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtEngineEditor::pbApplyClicked()
{
	guiGen.deserialize(currentEngine);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
