/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include<qmainwindow.h>
#include<qframe.h>
#include<qpushbutton.h>
#include<yade/lib-base/Logging.hpp>
#include"YadeQtGeneratedMainWindow.h"
#include<yade/gui-qt3/SimulationController.hpp>
#include<yade/gui-qt3/QtFileGenerator.hpp>
#include<yade/gui-qt3/QtSimulationPlayer.hpp>
#include<yade/gui-qt3/QtGUIPreferences.hpp>
#include<boost/thread/mutex.hpp>

class YadeQtMainWindow: public YadeQtGeneratedMainWindow
{
	public:
		shared_ptr<SimulationController> controller;
		shared_ptr<QtSimulationPlayer> player;
		shared_ptr<QtFileGenerator> generator;
		vector<shared_ptr<GLViewer> > glViews;
		shared_ptr<OpenGLRenderingEngine> renderer;
		size_t viewNo(shared_ptr<GLViewer>);
		boost::try_mutex glMutex;

		void ensureRenderer();
		void saveRendererConfig();
		
		void deleteSimulationController();
		void closeAllChilds();
		void createView();
		void closeView(GLViewer*);
		void closeView(int);
		size_t viewsSize(){return glViews.size();};
		void centerViews();
		void adjustCameraInCurrentView(qglviewer::Vec up,qglviewer::Vec dir);
		void loadSimulation(string file);
		void redrawAll(bool force=false);
		void lookDown(shared_ptr<GLViewer> glv);

	protected :
		shared_ptr<QtGUIPreferences> preferences;
		vector<shared_ptr<Factorable> > qtWidgets;

	public :
		YadeQtMainWindow ();
		virtual ~YadeQtMainWindow ();
		static YadeQtMainWindow* self; // retrieve instance pointer form elsewhere

		DECLARE_LOGGER;
	public slots :
		enum{EVENT_CONTROLLER=QEvent::User+1,EVENT_PLAYER,EVENT_VIEW,EVENT_GENERATOR,EVENT_RESTORE_GLVIEWER_NUM,EVENT_RESTORE_GLVIEWER_STR,EVENT_RESTORE_VIEWER_FILE,EVENT_RESIZE_VIEW};
		virtual void customEvent(QCustomEvent* e);
		/* each of player, controller, generator have slots for them being opened and closed: create{Player,Controller,Generator} and the instances are kept in player, controller, generator. */
		#define __MK_RM_CHILD(Child,child,YadeClass)  virtual void close##Child(){if(child)child=shared_ptr<YadeClass>();} virtual void create##Child(){closeAllChilds(); if(!child){child=shared_ptr<YadeClass>(new YadeClass()); connect(child.get(),SIGNAL(closeSignal()),this,SLOT(close##Child())); child->show();} else {child->show(); child->raise();}}
		__MK_RM_CHILD(Generator,generator,QtFileGenerator);
		__MK_RM_CHILD(Player,player,QtSimulationPlayer);
		__MK_RM_CHILD(Controller,controller,SimulationController);
		#undef __MK_RM_CHILD
		virtual void Quit();
		virtual void closeEvent(QCloseEvent * evt);
	protected:
		virtual void timerEvent(QTimerEvent* evt);

};

