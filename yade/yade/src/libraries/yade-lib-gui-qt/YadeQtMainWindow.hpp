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

#ifndef __YADEQTMAINWINDOW_H__
#define __YADEQTMAINWINDOW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qworkspace.h>
#include <qmainwindow.h>
#include <qframe.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "YadeQtGeneratedMainWindow.h"
#include "SimulationController.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class YadeQtMainWindow : public YadeQtGeneratedMainWindow
{
	protected : SimulationController * simulationController;
		
	protected : QWorkspace * workspace;
		
	protected : QPopupMenu *preprocessorMenu;
	protected : map<string,QPopupMenu*> menus;
	protected : vector<QAction*> items;
	protected : vector<shared_ptr<Factorable> > qtWidgets;

	// construction
	public : YadeQtMainWindow ();
	public : ~YadeQtMainWindow ();

	public : void addMenu(string menuName);
	public : void addItem(string menuName, string itemName);
	public : void createMenus();

	public slots : virtual void fileNewSimulation();
	public slots : virtual void fileExit();
	public slots : virtual void dynamicMenuClicked();
	public slots : virtual void closeSimulationControllerEvent();
	public slots : virtual void closeEvent(QCloseEvent * evt);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __YADEQTMAINWINDOW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
