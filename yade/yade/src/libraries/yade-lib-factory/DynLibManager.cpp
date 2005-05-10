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

#include "DynLibManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

DynLibManager::DynLibManager ( const string libName )
{
	baseDirs.clear();
	autoUnload = true;
	load(libName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

DynLibManager::DynLibManager ()
{
	baseDirs.clear();
	autoUnload = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

DynLibManager::~DynLibManager ()
{
	if (autoUnload)
		unloadAll();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void DynLibManager::addBaseDirectory(const string& dir)
{
	string tmpDir;
	if ( dir[dir.size()-1]=='/' || dir[dir.size()-1]=='\\' )
		tmpDir = dir.substr(0,dir.size()-1);
	else
		tmpDir = dir;

	baseDirs.push_back(tmpDir);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*Factory DynLibManager::resolve(const string libName, const string symb )
{
	if (isLoaded(libName))
	{
		string tmpSymb;
		tmpSymb = symb;
		tmpSymb.push_back('_');
		tmpSymb.append(libName);
		
		#ifdef WIN32
			void * sym = (void*)GetProcAddress(handles[libName], tmpSymb);
			if (sym==NULL)
				error();
			return (Factory)sym;
		#else
			void * sym = dlsym(handles[libName], tmpSymb.data());
		
			if (error())  
				return NULL;
			else
				return (Factory)sym;
		#endif
	}
	else
	{
		return NULL;
	}
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::load (const string libName )
{
	
	if (libName.empty())
		return false;

	string baseDir="";
	vector<string>::iterator bdi    = baseDirs.begin();
	vector<string>::iterator bdiEnd = baseDirs.end();
	for( ; bdi != bdiEnd ; ++bdi)
	{
		filesystem::path name = filesystem::path((*bdi)+"/"+libName);
		if ( filesystem::exists( name ) )
		{
			baseDir = (*bdi);
			break;
		}
	}

	if (autoUnload && isLoaded(libName))
		closeLib(libName);
		
	string file = baseDir;
		
	#ifdef WIN32
		file.append(libName);
		file.append(".dll");

		unsigned short * file2 = new unsigned short[file.length()+1];
		int i=0;
		while (file[i]!='\0')
		{
			file2[i] = file[i];
			i++;
		}
		file2[i] = '\0';

		HINSTANCE hinstLib = LoadLibrary(file2);

		if (hinstLib == NULL)
			return !error();
		else
		{
			handles[libName] = hinstLib;
			return true;
		}
	#else

		file.append("lib");
		file.append(libName);
		file.append(".so");

		void * handle = dlopen(file.data(), RTLD_NOW);

		if (!handle)
		{
			return !error();
		}
		else
		{
			handles[libName] = handle;
			return true;
		}
	#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::unload (const string libName)
{
	if (isLoaded(libName))
		return closeLib(libName);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::unloadAll ()
{
	#ifdef WIN32
		std::map<const string, HINSTANCE>::iterator ith  = handles.begin();
		std::map<const string, HINSTANCE>::iterator ithEnd  = handles.end();
	#else
		std::map<const string, void *>::iterator ith  = handles.begin();
		std::map<const string, void *>::iterator ithEnd  = handles.end();
	#endif

	for( ; ith!=ithEnd ; ++ith)
		if ((*ith).first.length()!=0)
			closeLib((*ith).first);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::isLoaded (const string libName)
{

	#ifdef WIN32
		std::map<const string, HINSTANCE>::iterator ith = handles.find(libName);	
	#else
		std::map<const string, void *>::iterator ith = handles.find(libName);	
	#endif

	return (ith!= handles.end() && (*ith).second!=NULL);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void DynLibManager::setAutoUnload ( bool enabled )
{
	autoUnload = enabled;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::closeLib(const string libName)
{
	#ifdef WIN32
		FreeLibrary(handles[libName]);
		return !error();
	#else	
		dlclose(handles[libName]);
		return !error();
	#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool DynLibManager::error() 
{    
	#ifdef WIN32    
    		TCHAR szBuf[80]; 
    		LPVOID lpMsgBuf;
    		DWORD dw = GetLastError(); 

    		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

		ofstream f; 
		f.open("log.txt");
		f << "Error " << dw << " : " << lpMsgBuf << endl;
		f.close();
		
		return (dw!=0);
	#else
		char * error = dlerror();
		if (error != NULL)  
		{
			cout << error << endl;
			//Omega::printErrorLog(error);
		}
		return (error!=NULL);
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
