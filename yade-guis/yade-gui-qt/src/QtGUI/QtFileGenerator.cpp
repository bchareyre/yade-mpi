/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtFileGenerator.hpp"
#include "FileDialog.hpp"
#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <yade/yade-lib-factory/ClassFactory.hpp>
#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-core/Omega.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include "MessageDialog.hpp"

QtFileGenerator::QtFileGenerator ( QWidget * parent , const char * name) : QtFileGeneratorController(parent,name)
{
	QSize s = size();
	setMinimumSize(s);
	setMaximumSize(QSize(s.width(),32000));	
	
	map<string,DynlibDescriptor>::const_iterator di    = Omega::instance().getDynlibsDescriptor().begin();
	map<string,DynlibDescriptor>::const_iterator diEnd = Omega::instance().getDynlibsDescriptor().end();
	for(;di!=diEnd;++di)
	{
		if (Omega::instance().isInheritingFrom((*di).first,"FileGenerator"))
			cbGeneratorName->insertItem((*di).first);
		if (Omega::instance().isInheritingFrom((*di).first,"IOFormatManager"))
			cbSerializationName->insertItem((*di).first);
	}
	setSerializationName("XMLFormatManager");
	
	leOutputFileName->setText("../data/scene.xml");

	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::AlwaysOn);
	scrollView->setHScrollBarMode(QScrollView::AlwaysOff);
	scrollViewLayout->addWidget( scrollView );
	scrollView->show();	

	cbGeneratorNameActivated(cbGeneratorName->currentText());
	cbGeneratorNameActivated(cbGeneratorName->currentText()); // FIXME : I need to call this function 2 times to have good display of scrollView
}


QtFileGenerator::~QtFileGenerator()
{

}

void QtFileGenerator::setSerializationName(string n)
{
	for(int i=0 ; i<cbSerializationName->count() ; ++i)
	{
		cbSerializationName->setCurrentItem(i);
		if(cbSerializationName->currentText() == n)
			return;
	}
}

void QtFileGenerator::setGeneratorName(string n)
{
	for(int i=0 ; i<cbGeneratorName->count() ; ++i)
	{
		cbGeneratorName->setCurrentItem(i);
		if(cbGeneratorName->currentText() == n)
			return;
	}
}

void QtFileGenerator::pbChooseClicked()
{
	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("Yade Binary File (*.yade)");
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getSaveFileName("../data", filters , "Choose a file to save", this->parentWidget()->parentWidget(),selectedFilter );

	if (fileName.size()!=0 && selectedFilter == "XML Yade File (*.xml)" && fileName!="/" )
	{
		setSerializationName("XMLFormatManager");
		leOutputFileName->setText(fileName);
	}
	else if (fileName.size()!=0 && selectedFilter == "Yade Binary File (*.yade)" && fileName!="/" )
	{
		setSerializationName("BINFormatManager");
		leOutputFileName->setText(fileName);
	}
}

void QtFileGenerator::displayFileGeneratorAttributes(shared_ptr<FileGenerator>& fg)
{
	try
	{ 
		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(false);
		guiGen.setShift(10,10);
		guiGen.setShowButtons(false);
		
		QSize s = scrollView->size();
		QPoint p = scrollView->pos();	

		delete scrollViewFrame;
		scrollViewFrame = new QFrame();
		scrollViewFrame->resize(s.width()-17,s.height()); // -17 because of the size of the scrollbar
		
		gbGeneratorParameters->setTitle(fg->getClassName()+" Parameters");
		
		guiGen.buildGUI(fg,scrollViewFrame);
			
			
		if (s.height()>scrollViewFrame->size().height())
		{
			scrollViewFrame->setMinimumSize(s.width()-17,s.height());
			scrollViewFrame->setMaximumSize(s.width()-17,s.height());
		}
			
		scrollView->addChild(scrollViewFrame);
		scrollViewFrame->show();
	}
	catch (FactoryError& e)
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(string("Error: ") + e.what(),this->parentWidget()->parentWidget()));
		md->exec();
	}
}

void QtFileGenerator::cbGeneratorNameActivated(const QString& name)
{
	shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(name));
	displayFileGeneratorAttributes(fg);
}

void QtFileGenerator::cbSerializationNameActivated(const QString& s)
{
	string fileName = leOutputFileName->text();
	string ext;
	if( s == "BINFormatManager")
		ext = ".yade";
	else if ( s == "XMLFormatManager")
		ext = ".xml";
	else ext = ".unknownFormat";
	if( filesystem::extension(fileName) != "" )
	{
		filesystem::path p = filesystem::change_extension(fileName,ext);
		leOutputFileName->setText(p.string());
	}
}

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


void QtFileGenerator::pbCloseClicked()
{
	close();
	destroy();
}


void QtFileGenerator::pbLoadClicked()
{
	shared_ptr<FileGenerator> fg;

	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getOpenFileName("../data", filters, "Choose a FileGenerator configuration to load", this->parentWidget()->parentWidget(), selectedFilter );
	if ( 	   fileName.size()!=0 
		&& (selectedFilter == "XML Yade File (*.xml)") 
		&& filesystem::exists(fileName) 
		&& (filesystem::extension(fileName)==".xml"))
	{
		try
		{
			IOFormatManager::loadFromFile("XMLFormatManager",fileName,"fileGenerator",fg); 
		} 
		catch(SerializableError& e) // catching it...
		{
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(string("FileGenerator failed to load: ") + e.what(),this->parentWidget()->parentWidget()));
			md->exec();
			return;
		}
	}
	setGeneratorName(fg->getClassName());
	displayFileGeneratorAttributes(fg);
}



void QtFileGenerator::pbSaveClicked()
{
	// FIXME add some test to avoid crashing
	shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(cbGeneratorName->currentText()));
	
	fg->setFileName(leOutputFileName->text());
	fg->setSerializationLibrary(cbSerializationName->currentText());
	guiGen.deserialize(fg);
	
	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("XML Yade File (*.xml)");
	string title = "Save FileGenerator \"" + fg->getClassName() + "\" configuration";
	string fileName = FileDialog::getSaveFileName("../data", filters, title, this->parentWidget()->parentWidget(), selectedFilter );

	if ( 	   fileName.size()!=0
		&& (selectedFilter == "XML Yade File (*.xml)") 
		&& (filesystem::extension(fileName)==".xml" || filesystem::extension(fileName)=="" )
		&& (fileName != "")
		&& (fileName != "/")
		&& (fileName != "../data"))
	{
		if(filesystem::extension(fileName)=="") // user forgot to specify extension - fix it.
			fileName += ".xml";
		
		cerr << "saving FileGenerator configuration: " << fileName << "\n";
		IOFormatManager::saveToFile("XMLFormatManager",fileName,"fileGenerator",fg); 
	}
	else
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("Save failed - bad file extension.",this->parentWidget()->parentWidget()));
		md->exec(); 
	}
}



void QtFileGenerator::closeEvent(QCloseEvent *evt)
{
	close();
	QtFileGeneratorController::closeEvent(evt);
}

