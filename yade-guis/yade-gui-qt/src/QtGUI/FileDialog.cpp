/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FileDialog.hpp"
#include <boost/shared_ptr.hpp>

using namespace boost;

FileDialog::FileDialog (const QString& directory,const QString& filters,const string& caption,QWidget* parent) : QFileDialog(directory,filters,parent,"FileDialog",false)
{	
	setCaption(caption);
	
	QSize s1 = parent->size();
	QSize s2 = size();
	QSize s = s1-s2;
	reparent(parent,QPoint(s.width()/2,s.height()/2));	
	show();

}


FileDialog::~FileDialog ()
{

}


string FileDialog::getOpenFileName(const string& directory,const vector<string>& filters,const string& caption,QWidget* parent,string& selectedFilter )
{
	const QString d(directory);
	const QString f(filters[0]);
	
	shared_ptr<FileDialog> fd = shared_ptr<FileDialog>(new FileDialog(d,f,caption,parent));
	for(int i = 1 ; i<filters.size() ; ++i)
		fd->addFilter(filters[i]);
	fd->exec();
	selectedFilter = fd->selectedFilter().data();
	return fd->selectedFile().data();
}


string FileDialog::getSaveFileName(const string& directory,const vector<string>& filters,const string& caption,QWidget* parent,string& selectedFilter )
{
	const QString d(directory);
	const QString f(filters[0]);
		
	shared_ptr<FileDialog> fd = shared_ptr<FileDialog>(new FileDialog(d,f,caption,parent));
	fd->setMode( QFileDialog::AnyFile );
	for(int i = 1 ; i<filters.size() ; ++i)
		fd->addFilter(filters[i]);
	fd->exec();
	selectedFilter = fd->selectedFilter().data();
	return fd->selectedFile().data();
}


string FileDialog::getExistingDirectory ( const string& directory,const string& caption,QWidget* parent)
{
	const QString d(directory.c_str());
		
	shared_ptr<FileDialog> fd = shared_ptr<FileDialog>(new FileDialog(d,"",caption,parent));
	fd->setMode( QFileDialog::DirectoryOnly );
	fd->exec();
	return fd->selectedFile().data();
}

