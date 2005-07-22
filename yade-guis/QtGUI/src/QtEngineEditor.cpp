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
	map<string,DynlibType>::const_iterator di    = Omega::instance().getDynlibsType().begin();
	map<string,DynlibType>::const_iterator diEnd = Omega::instance().getDynlibsType().end();
	for(;di!=diEnd;++di)
	{
		if ((*di).second.baseClass=="Engine" || (*di).second.baseClass=="MetaEngine" || (*di).second.baseClass=="TimeStepper")
			cbEnginesList->insertItem((*di).first);
	}

	connect( glEngineEditor, SIGNAL( verifyValidity() ), this, SLOT( verifyValidity() ) );
	connect( glEngineEditor, SIGNAL( engineSelected() ), this, SLOT( engineSelected() ) );


	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::AlwaysOn);
	scrollView->setHScrollBarMode(QScrollView::AlwaysOff);
	scrollViewLayout->addWidget( scrollView );
	scrollView->show();	


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
	glEngineEditor->addEngine(cbEnginesList->currentText());
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

void QtEngineEditor::engineSelected()
{

	try
	{
		//FIXME dynamic_cast is not working ???
		shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared("PhysicalActionApplier"));

		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(false);
		guiGen.setShift(10,10);
		guiGen.setShowButtons(false);
		
		QSize s = scrollView->size();
		QPoint p = scrollView->pos();	

		delete scrollViewFrame;
		scrollViewFrame = new QFrame();
		scrollViewFrame->resize(s.width()-17,s.height()); // -17 because of the size of the scrollbar

		guiGen.buildGUI(fg,scrollViewFrame);
			
		if (s.height()>scrollViewFrame->size().height())
		{
			scrollViewFrame->setMinimumSize(s.width()-17,s.height());
			scrollViewFrame->setMaximumSize(s.width()-17,s.height());
		}
			
		scrollView->addChild(scrollViewFrame);
		scrollViewFrame->show();
	}
	catch (FactoryError&)
	{
	
	}
}