/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTPREFERENCESEDITOR_HPP
#define QTPREFERENCESEDITOR_HPP

#include <QtGeneratedPreferencesEditor.h>
#include<yade/lib-factory/Factorable.hpp>

class QtPreferencesEditor : public QtGeneratedPreferencesEditor, public Factorable
{
	private :
		bool testDirectory(const string& dirName);
		void savePreferences();
		void loadPreferences();
		void scanPlugins();
		void buildPluginsListView();

	protected :
		void closeEvent(QCloseEvent *evt);

	public :
		QtPreferencesEditor (QWidget * parent = 0, const char * name = 0 );
		virtual ~QtPreferencesEditor ();
	
	public slots :
		virtual void lbPreferencesListHighlighted(int i);
	
 		virtual void pbAddIncludeFolderClicked();
		virtual void pbDeleteIncludeFolderClicked();
		virtual void pbIncludePathClicked();
		virtual void lvIncludeFoldersSelectionChanged(QListViewItem* lvi);
		virtual void leIncludeFolderReturnPressed();

 		virtual void pbAddPluginFolderClicked();
		virtual void pbDeletePluginFolderClicked();
		virtual void pbPluginPathClicked();
		virtual void lvPluginFoldersSelectionChanged(QListViewItem* lvi);
		virtual void lePluginFolderReturnPressed();

		virtual void pbRescanPluginsClicked();	
	
	REGISTER_CLASS_NAME(QtPreferencesEditor);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

REGISTER_FACTORABLE(QtPreferencesEditor);

#endif // QTPREFERENCESEDITOR_HPP

