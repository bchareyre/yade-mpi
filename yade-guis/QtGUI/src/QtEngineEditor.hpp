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

#ifndef __QTENGINEEDITOR_HPP__
#define __QTENGINEEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGeneratedEngineEditor.h>
#include <qlayout.h>
#include <qframe.h>
#include <qscrollview.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-factory/Factorable.hpp>
#include <yade/yade-lib-serialization-qt/QtGUIGenerator.hpp>
#include <yade/yade-core/Engine.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "QtMetaDispatchingEngineProperties.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
@author Olivier Galizzi
*/
class QtEngineEditor : public QtGeneratedEngineEditor, public Factorable
{

	private : QtGUIGenerator guiGen;	
	private : QFrame * engineFrame;
	private : QtMetaDispatchingEngineProperties * metaDispatchingEngineFrame;

	enum EngineType { STANDALONEENGINE, DEUSEXMACHINA, METADISPATCHINGENGINE1D, METADISPATCHINGENGINE2D, METAENGINE };
	private : typedef struct EngineDescriptor
		  {
			shared_ptr<Engine> engine;
			EngineType type;
			
		  } EngineDescriptor;

	private : map<int,EngineDescriptor> engines;
	private : shared_ptr<Engine> currentEngine;

	public : QtEngineEditor();
	public : ~QtEngineEditor();

	public slots : void pbAddEngineClicked();
	public slots : void pbAddMetaEngineClicked();
	public slots : void pbAddDeusExMachinaClicked();

	public slots : void pbSaveClicked();
	public slots : void pbLoadClicked();
	public slots : void pbPathClicked();
	public slots : void pbApplyClicked();


	public slots : void verifyValidity();
	public slots : void engineSelected(int i);
	public slots : void deleteEngine(int i);
	
	REGISTER_CLASS_NAME(QtEngineEditor);
	REGISTER_BASE_CLASS_NAME(Factorable);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(QtEngineEditor);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTENGINEEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
