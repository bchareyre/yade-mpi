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

#include "FileDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FileDialog::FileDialog (const QString& directory,const QString& filters,const string& caption,QWidget* parent) : QFileDialog(directory,filters,parent,"FileDialog",false)
{	
	setCaption(caption);
	
	QSize s1 = parent->size();
	QSize s2 = size();
	QSize s = s1-s2;
	reparent(parent,QPoint(s.width()/2,s.height()/2));	
	show();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FileDialog::~FileDialog ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string FileDialog::getOpenFileName(const string& directory,const string& filters,const string& caption,QWidget* parent,string& selectedFilter )
{
	const QString d(directory.c_str());
	const QString f(filters.c_str());
	
	shared_ptr<FileDialog> fd = shared_ptr<FileDialog>(new FileDialog(d,f,caption,parent));
	fd->exec();
	selectedFilter = fd->selectedFilter().data();
	return fd->selectedFile().data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string FileDialog::getSaveFileName(const string& directory,const string& filters,const string& caption,QWidget* parent,string& selectedFilter )
{
	const QString d(directory.c_str());
	const QString f(filters.c_str());
		
	shared_ptr<FileDialog> fd = shared_ptr<FileDialog>(new FileDialog(d,f,caption,parent));
	fd->setMode( QFileDialog::AnyFile );
	fd->exec();
	selectedFilter = fd->selectedFilter().data();
	return fd->selectedFile().data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
