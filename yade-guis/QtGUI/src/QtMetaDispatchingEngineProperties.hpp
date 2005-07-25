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

#ifndef __QTMETADISPATCHINGENGINEPROPERTIES_HPP__
#define __QTMETADISPATCHINGENGINEPROPERTIES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGeneratedMetaDispatchingEngineProperties.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpixmap.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Engine.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
@author Olivier Galizzi
*/
class QtMetaDispatchingEngineProperties : public QtGeneratedMetaDispatchingEngineProperties
{
	protected : vector<vector<QWidget*> > cbs;
	protected : string baseClass1;
	protected : string baseClass2;
	protected : string baseFunctor;
	protected : int dimension;

	protected : vector<vector<string> > baseClasses;

	protected : void buildDynlibList();

	private   : QPixmap image0;
	private   : QPixmap image1;

	public    : QtMetaDispatchingEngineProperties(const string& baseClass1, const string& baseClass2, const string& baseFunctor, QWidget* parent=0,  const char* name=0 );
	public    : QtMetaDispatchingEngineProperties(const string& baseClass1, const string& baseFunctor, QWidget* parent=0,  const char* name=0 );
	public    : QtMetaDispatchingEngineProperties(QWidget* parent=0,  const char* name=0);
	public    : ~QtMetaDispatchingEngineProperties();

	public slots : void pbAddClicked();
	public slots : void pbOkClicked();
	public slots : void pbRemoveClicked();
	public slots : void pbSerializationClicked();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTMETADISPATCHINGENGINEPROPERTIES_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
