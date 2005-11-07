/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTMETADISPATCHINGENGINEPROPERTIES_HPP
#define QTMETADISPATCHINGENGINEPROPERTIES_HPP

#include <QtGeneratedMetaDispatchingEngineProperties.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <yade/yade-core/MetaDispatchingEngine.hpp>
#include <yade/yade-core/Engine.hpp>
#include <yade/yade-lib-serialization-qt/QtGUIGenerator.hpp>

class QtMetaDispatchingEngineProperties : public QtGeneratedMetaDispatchingEngineProperties
{
	protected :
		vector<vector<QWidget*> > cbs;
		vector<shared_ptr<EngineUnit> > engineUnitParameters;

		vector<string> baseClasses;
		int dimension;
		QtGUIGenerator guiGen;
		QFrame * engineUnitFrame;
		shared_ptr<MetaDispatchingEngine> metaEngine;

		vector<vector<string> > inheritedClasses;

		void buildDynlibList();

	private :
		QPixmap image0;
		QPixmap image1;

	public :
		QtMetaDispatchingEngineProperties(shared_ptr<MetaDispatchingEngine>& mde, QWidget* parent=0,  const char* name=0 );
		QtMetaDispatchingEngineProperties(QWidget* parent=0,  const char* name=0);
		~QtMetaDispatchingEngineProperties();

	public slots :
		void pbAddClicked();
		void pbOkClicked();
		void pbRemoveClicked();
		void pbSerializationClicked();

	protected  : void showEvent( QShowEvent * );
};

#endif // QTMETADISPATCHINGENGINEPROPERTIES_HPP

