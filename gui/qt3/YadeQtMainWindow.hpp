/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef YADEQTMAINWINDOW_HPP
#define YADEQTMAINWINDOW_HPP

/* #define this if you want to have workspace: one toplevel windows with subwindows inside it;
 * that is the old behavior which is now deprecated, but still supported.
 *
 * If it is not defined, all dialogues etc will be standalone windows.
 *
 */

// #define USE_WORKSPACE

#include <qworkspace.h>
#include <qmainwindow.h>
#include <qframe.h>
#include<yade/lib-base/Logging.hpp>
#include "YadeQtGeneratedMainWindow.h"
#include "SimulationController.hpp"
#include "QtGUIPreferences.hpp"

class YadeQtMainWindow : public YadeQtGeneratedMainWindow
{
	protected :
		shared_ptr<QtGUIPreferences> preferences;
		SimulationController * simulationController;
		#ifdef USE_WORKSPACE
			QWorkspace * workspace;
		#endif
		
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

		static YadeQtMainWindow* self; // HACK to retrieve this "singleton" form elsewhere

		void addMenu(string menuName);
		void addItem(string menuName, string itemName,string className);
		void createMenus();
		DECLARE_LOGGER;

	public slots :
		virtual void fileNewSimulation();
		virtual void fileExit();
		virtual void dynamicMenuClicked();
		virtual void closeSimulationControllerEvent();
		virtual void closeEvent(QCloseEvent * evt);
};

#endif // __YADEQTMAINWINDOW_H__

