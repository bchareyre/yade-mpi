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

#ifndef __QTPREFERENCESEDITOR_HPP__
#define __QTPREFERENCESEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGeneratedPreferencesEditor.h>
#include <yade-lib-factory/Factorable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief 

	
*/
class QtPreferencesEditor : public QtGeneratedPreferencesEditor, public Factorable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*! Constructor */
	public : QtPreferencesEditor (QWidget * parent = 0, const char * name = 0 );

	/*! Destructor */
	public : virtual ~QtPreferencesEditor ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : bool testDirectory(const string& dirName);
	private : void savePreferences();
	private : void loadPreferences();
	private : void scanPlugins();

	protected : void closeEvent(QCloseEvent *evt);

	public slots : virtual void lbPreferencesListHighlighted(int i);
	
 	public slots : virtual void pbAddIncludeFolderClicked();
	public slots : virtual void pbDeleteIncludeFolderClicked();
	public slots : virtual void pbIncludePathClicked();
	public slots : virtual void lvIncludeFoldersSelectionChanged(QListViewItem* lvi);
	public slots : virtual void leIncludeFolderReturnPressed();

 	public slots : virtual void pbAddPluginFolderClicked();
	public slots : virtual void pbDeletePluginFolderClicked();
	public slots : virtual void pbPluginPathClicked();
	public slots : virtual void lvPluginFoldersSelectionChanged(QListViewItem* lvi);
	public slots : virtual void lePluginFolderReturnPressed();

	public slots : virtual void pbRescanPluginsClicked();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(QtPreferencesEditor);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTPREFERENCESEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

