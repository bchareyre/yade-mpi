/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTFILEGENERATOR_HPP
#define QTFILEGENERATOR_HPP

#include <qlayout.h>
#include <qframe.h>
#include <qscrollview.h>
#include <QtFileGeneratorController.h>
#include<yade/lib-factory/Factorable.hpp>
#include<yade/lib-serialization-qt/QtGUIGenerator.hpp>

class FileGenerator;
class ThreadRunner;

class QtFileGenerator : public QtFileGeneratorController, public Factorable
{
	private :
		QtGUIGenerator	guiGen;	
		QScrollView *	scrollView;
		QFrame *	scrollViewFrame;
		QVBoxLayout*	scrollViewLayout;
		void setSerializationName(string);
		void setGeneratorName(string n);
		void displayFileGeneratorAttributes(shared_ptr<FileGenerator>&);
		
		shared_ptr<ThreadRunner>	 m_runner;
		shared_ptr<FileGenerator>	 m_worker;

	public :
		QtFileGenerator (QWidget * parent = 0, const char * name = 0 );
		virtual ~QtFileGenerator ();

	public slots :
		virtual void pbChooseClicked(); 
		virtual void cbGeneratorNameActivated(const QString& s);
		virtual void cbSerializationNameActivated(const QString& s);
		virtual void pbGenerateClicked(); 
		virtual void pbCloseClicked(); // FIXME - stupid qt3-designer. This is now "Stop" not "Close"
		virtual void pbLoadClicked();
		virtual void pbSaveClicked();
	
	protected : 
		virtual void closeEvent(QCloseEvent *evt);
                virtual void timerEvent(QTimerEvent* );
	
	REGISTER_CLASS_NAME(QtFileGenerator);
	REGISTER_BASE_CLASS_NAME(Factorable);

};

REGISTER_FACTORABLE(QtFileGenerator);

#endif // QTFILEGENERATOR_HPP

