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

#ifndef __DYNLIBMANAGER_H__
#define __DYNLIBMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
	#define OS "Windows"
	#include <stdio.h> 
	#include <windows.h> 
	typedef int (*MYPROC)(LPTSTR); 
#else
	#include <dlfcn.h>
	#define OS "Linux"
#endif

#include <string>
#include <iostream>
#include <map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class DynLibManager 
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Types											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	#ifdef WIN32
		private : std::map<const string, HINSTANCE> handles;	
	#else	
		private : std::map<const string, void *> handles;
	#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : vector<string> baseDirs;
	public : void addBaseDirectory(const string& dir);
	
	private : bool autoUnload;
	
	// construction
	public : DynLibManager ();
	public : DynLibManager (const string libName);
	public : ~DynLibManager ();


//	public : Factory resolve (const string libName, const string symb );
	public : bool load (const string libName);
	public : bool unload (const string libName);
	public : bool isLoaded (const string libName);
	public : bool unloadAll ();
	public : void setAutoUnload ( bool enabled );
	private : bool closeLib(const string libName);
	private : bool error();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __DYNLIBMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
