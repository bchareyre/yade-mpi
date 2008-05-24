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
#include<yade/lib-base/Logging.hpp>
#include"YadeQtGeneratedMainWindow.h"
#include"SimulationController.hpp"
#include"QtGUIPreferences.hpp"

class YadeQtMainWindow : public YadeQtGeneratedMainWindow
{
	public:
		shared_ptr<SimulationController> simulationController;
		vector<shared_ptr<GLViewer> > glViews;
		shared_ptr<RenderingEngine> renderer;
		void ensureRenderer();
		void saveRendererConfig();
		
		void deleteSimulationController();
		void createView();
		void closeView(GLViewer*);
		void closeView(int);
		void centerViews();
		void loadSimulation(string file);


	protected :
		shared_ptr<QtGUIPreferences> preferences;
		vector<shared_ptr<Factorable> > qtWidgets;

	private :
		bool menuNameExists(const string name);
		QPopupMenu* getPopupMenu(const string name);

	public :
		YadeQtMainWindow ();
		virtual ~YadeQtMainWindow ();

		static YadeQtMainWindow* self; // HACK to retrieve this "singleton" form elsewhere

		DECLARE_LOGGER;

	public slots :
		virtual void createGenerator();
		virtual void createPlayer();
		virtual void createSimulationController();
		virtual void Quit();
		virtual void closeSimulationControllerEvent();
		virtual void closeEvent(QCloseEvent * evt);
	protected:
		virtual void timerEvent(QTimerEvent* evt);
};

