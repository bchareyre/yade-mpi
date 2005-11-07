/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADEQTMAINWINDOW_HPP
#define YADEQTMAINWINDOW_HPP

#include <qworkspace.h>
#include <qmainwindow.h>
#include <qframe.h>
#include "YadeQtGeneratedMainWindow.h"
#include "SimulationController.hpp"
#include "QtGUIPreferences.hpp"

class YadeQtMainWindow : public YadeQtGeneratedMainWindow
{
	protected :
		shared_ptr<QtGUIPreferences> preferences;
		SimulationController * simulationController;
		QWorkspace * workspace;
		
		QPopupMenu *preprocessorMenu;
		vector<pair<string,QPopupMenu*> > menus;
		vector<QAction*> items;
		vector<shared_ptr<Factorable> > qtWidgets;
		map<string,string> item2ClassName;

	private :
		bool menuNameExists(const string name);
		QPopupMenu* getPopupMenu(const string name);

	public :
		YadeQtMainWindow ();
		virtual ~YadeQtMainWindow ();

		void addMenu(string menuName);
		void addItem(string menuName, string itemName,string className);
		void createMenus();

	public slots :
		virtual void fileNewSimulation();
		virtual void fileExit();
		virtual void dynamicMenuClicked();
		virtual void closeSimulationControllerEvent();
		virtual void closeEvent(QCloseEvent * evt);
};

#endif // __YADEQTMAINWINDOW_H__

