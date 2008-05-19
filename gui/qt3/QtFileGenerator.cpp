/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/lib-factory/ClassFactory.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/ThreadRunner.hpp>
#include "QtFileGenerator.hpp"
#include "FileDialog.hpp"
#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include "MessageDialog.hpp"
#include "YadeQtMainWindow.hpp"

QtFileGenerator::QtFileGenerator ( QWidget * parent , const char * name)
	: QtFileGeneratorController(parent,name)
	, m_runner(shared_ptr<ThreadRunner>())
	, m_worker(shared_ptr<FileGenerator>())
{
	QSize s = size();
	setMinimumSize(s);
	setMaximumSize(QSize(s.width(),32000));	
	textLabel1->setText("waiting for orders");
	
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
	
	leOutputFileName->setText("./scene.xml");

	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::AlwaysOn);
	scrollView->setHScrollBarMode(QScrollView::AlwaysOff);
	scrollViewLayout->addWidget( scrollView );
	scrollView->show();	

	cbGeneratorNameActivated(cbGeneratorName->currentText());
	cbGeneratorNameActivated(cbGeneratorName->currentText()); // FIXME : I need to call this function 2 times to have good display of scrollView

	pbClose->setEnabled(false);
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
	string fileName = FileDialog::getSaveFileName(".", filters , "Choose a file to save", this->parentWidget()->parentWidget(),selectedFilter );

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
	leOutputFileName->setText(fg->getFileName());
}

void QtFileGenerator::cbGeneratorNameActivated(const QString& name)
{
//	try { // FIXME - should it be here or not?
	shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(name));
/* 	}
	catch (FactoryError& e)
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(string("Error: ") + e.what(),this->parentWidget()->parentWidget()));
		md->exec();
	}
*/
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

#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

void QtFileGenerator::pbGenerateClicked()
{
	if(m_worker != 0)
		return;

	m_worker = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(cbGeneratorName->currentText()));
	
	guiGen.deserialize(m_worker);

	m_worker->setFileName(leOutputFileName->text());
	m_worker->setSerializationLibrary(cbSerializationName->currentText());
		
	m_runner   = shared_ptr<ThreadRunner>(new ThreadRunner(m_worker.get()));
	m_runner->spawnSingleAction();

//	string message = fg->generateAndSave();
	pbClose->setEnabled(true);
	pbGenerate->setEnabled(false);
	startTimer(20);
	progressBar1->setTotalSteps(1000);
}

void QtFileGenerator::timerEvent( QTimerEvent* )
{
	// generating ....
	if(m_worker && m_runner)
	{
		textLabel1->setText(m_worker->getStatus());
		progressBar1->setProgress((int)(m_worker->progress()*1000.0));
	}

	// generation finished
	if(m_worker && m_runner && !m_runner->looping() && m_worker->done())
	{
		m_runner   = shared_ptr<ThreadRunner>();
		pbClose->setEnabled(false);
		pbGenerate->setEnabled(true);

		bool successfullyGenerated=boost::any_cast<bool>(m_worker->getReturnValue());
		string message=m_worker->message;

		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(string(successfullyGenerated?"SUCCESS:\n\n":"FAILURE!\n\n")+message,
		#ifdef USE_WORKSPACE
			this->parentWidget()->parentWidget()
		#else
			NULL
		#endif
		));
		md->exec();

		m_worker=shared_ptr<FileGenerator>();

		killTimers();
		progressBar1->reset();
		textLabel1->setText("waiting for orders");

		/* now, launch the generated simulation if
		 * 1. it is desired,
		 * 2. generation was successful
		 * 3. no simulation is open already (by checking Omega's simulationFileName) */
		if(cbOpenAutomatically->isChecked() 
			&& successfullyGenerated //filesystem::exists(filesystem::path((const char*)(leOutputFileName->text()))) 
			&& Omega::instance().getSimulationFileName()=="")
		{
			//QWidget* qw=this; while (qw->parentWidget()) qw=qw->parentWidget(); // find toplevel widget - which should be yade's main window
			Omega::instance().setSimulationFileName((const char*)(leOutputFileName->text()));
			//(dynamic_cast<YadeQtMainWindow*>(qw))->fileNewSimulation();
			dynamic_cast<YadeQtMainWindow*>(YadeQtMainWindow::self)->fileNewSimulation();
		}
	}
}

// FIXME - stupid qt3-designer. This is now "Stop" not "Close"
void QtFileGenerator::pbCloseClicked()
{

	if(m_runner)
		m_runner->pleaseTerminate();

	// So here we want to kill the generator thread.
//	std::cerr << "Stop\n";


///////////////////// FIXME - this Close was doing.
//	close();
//	destroy();
///////////////////////////////////////////////////
}

void QtFileGenerator::pbLoadClicked()
{
	shared_ptr<FileGenerator> fg;

	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getOpenFileName(".", filters, "Choose a FileGenerator configuration to load", 
		#ifdef USE_WORKSPACE
			this->parentWidget()->parentWidget()
		#else
			NULL
		#endif
		, selectedFilter );
	if ( 	   fileName.size()!=0 
		&& (selectedFilter == "XML Yade File (*.xml)") 
		&& filesystem::exists(fileName) 
		&& (filesystem::extension(fileName)==".xml"))
	{
		try
		{
			IOFormatManager::loadFromFile("XMLFormatManager",fileName,"fileGenerator",fg); 
			setGeneratorName(fg->getClassName());
			displayFileGeneratorAttributes(fg);

			std::string tmp=fg->getFileName();
			if(tmp!="./scene.xml") // this check to avoid resetting data, when loading older file.
			{
				leOutputFileName->setText(tmp);
				setSerializationName(fg->getSerializationLibrary());
			}
		} 
		catch(SerializableError& e) // catching it...
		{
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(string("FileGenerator failed to load: ") + e.what(),
			#ifdef USE_WORKSPACE
				this->parentWidget()->parentWidget()
			#else
				NULL
			#endif	
				));
			md->exec();
			return;
		}
	}
}

void QtFileGenerator::pbSaveClicked()
{
	// FIXME add some test to avoid crashing
	shared_ptr<FileGenerator> fg = static_pointer_cast<FileGenerator>(ClassFactory::instance().createShared(cbGeneratorName->currentText()));
	
	guiGen.deserialize(fg);
	fg->setFileName(leOutputFileName->text());
	fg->setSerializationLibrary(cbSerializationName->currentText());
	
	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("XML Yade File (*.xml)");
	string title = "Save FileGenerator \"" + fg->getClassName() + "\" configuration";
	string fileName = FileDialog::getSaveFileName(".", filters, title, this->parentWidget()->parentWidget(), selectedFilter );

	if ( 	   fileName.size()!=0
		&& (selectedFilter == "XML Yade File (*.xml)") 
		&& (filesystem::extension(fileName)==".xml" || filesystem::extension(fileName)=="" )
		&& (fileName != "")
		&& (fileName != "/")
		&& (fileName != "."))
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
	if(m_worker || m_runner)
		return;
	close();
	QtFileGeneratorController::closeEvent(evt);
}

