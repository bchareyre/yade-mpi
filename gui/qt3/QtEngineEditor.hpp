/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTENGINEEDITOR_HPP
#define QTENGINEEDITOR_HPP

#include <QtGeneratedEngineEditor.h>
#include <qlayout.h>
#include <qframe.h>
#include <qscrollview.h>
#include<yade/lib-factory/Factorable.hpp>
#include<yade/lib-serialization-qt/QtGUIGenerator.hpp>
#include<yade/core/Engine.hpp>
#include "QtMetaDispatchingEngineProperties.hpp"

class QtEngineEditor : public QtGeneratedEngineEditor, public Factorable
{
	private :
		QtGUIGenerator guiGen;	
		QFrame * engineFrame;

		enum EngineType { STANDALONEENGINE, DEUSEXMACHINA, METADISPATCHINGENGINE1D, METADISPATCHINGENGINE2D, METAENGINE };
		struct EngineDescriptor
		{
			shared_ptr<Engine> engine;
			EngineType type;
		};

		vector<shared_ptr<Engine> > enginesVec;
	
		map<int,EngineDescriptor> engines;
		shared_ptr<Engine> currentEngine;

	public :
		QtEngineEditor();
		~QtEngineEditor();

	public slots :
		void pbAddEngineClicked();
		void pbAddMetaEngineClicked();
		void pbAddDeusExMachinaClicked();

		void pbSaveClicked();
		void pbLoadClicked();
		void pbPathClicked();

		void verifyValidity();
		void engineSelected(int i);
		void deleteEngine(int i);
	
 	protected :
		void closeEvent(QCloseEvent *);
	REGISTER_CLASS_NAME(QtEngineEditor);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

REGISTER_FACTORABLE(QtEngineEditor);

#endif // QTENGINEEDITOR_HPP

