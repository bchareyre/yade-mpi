/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  with help from Bronek Kozicki                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "DynLibManager.hpp"


#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>


#include "ClassFactory.hpp"


using namespace std;
using namespace boost;


DynLibManager::DynLibManager ()
{
	baseDirs.clear();
	autoUnload = true;
}


DynLibManager::~DynLibManager ()
{
	if (autoUnload)
		unloadAll();
}


void DynLibManager::addBaseDirectory(const string& dir)
{
	string tmpDir;
	if ( dir[dir.size()-1]=='/' || dir[dir.size()-1]=='\\' )
		tmpDir = dir.substr(0,dir.size()-1);
	else
		tmpDir = dir;

	baseDirs.push_back(tmpDir);
}


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


bool DynLibManager::loadFromDirectoryList (const string& libName )
{
	if (libName.empty())
		return false;

	string libFileName = libNameToSystemName(libName);

	string baseDir = findLibDir(libName);

	string fullLibName;
	if (baseDir.length()==0)
		return load(libFileName,libName);
	else
		return load(baseDir+"/"+libFileName,libName);
	
}


bool DynLibManager::load (const string& fullLibName, const string& libName )
{
	if (libName.empty() || fullLibName.empty())
		return false;

#ifdef WIN32
	if (isLoaded(libName))
		return true;

	HINSTANCE handle = LoadLibraryA(fullLibName.c_str());
#else
	void * handle = dlopen(fullLibName.data(), RTLD_NOW);
#endif

	if (!handle)
		return !error();

	handles[libName] = handle;
	return true;
}


bool DynLibManager::unload (const string libName)
{
	if (isLoaded(libName))
	#ifdef WIN32
		return FreeLibrary(handles[libName]);
	#else
		return closeLib(libName);
	#endif		
	else
		return false;
}


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
			unload((*ith).first);
	return false;
}


bool DynLibManager::isLoaded (const string libName)
{

	#ifdef WIN32
		std::map<const string, HINSTANCE>::iterator ith = handles.find(libName);	
	#else
		std::map<const string, void *>::iterator ith = handles.find(libName);	
	#endif

	return (ith!= handles.end() && (*ith).second!=NULL);

}


void DynLibManager::setAutoUnload ( bool enabled )
{
	autoUnload = enabled;
}


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


bool DynLibManager::error() 
{
	#ifdef WIN32
		char* lpMsgBuf;
		const DWORD lastError = GetLastError();
	
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &lpMsgBuf, 0, NULL);
	
		std::string errMsg(lpMsgBuf);
		LocalFree(lpMsgBuf); // mind the FORMAT_MESSAGE_ALLOCATE_BUFFER !
	
		if (lastError != ERROR_SUCCESS)
		{
//			cerr << errMsg << endl;
//			Omega::printErrorLog(errMsg);
			return true;
		}
	
		return false;
	#else
 		char * error = dlerror();
		if (error != NULL)  
		{
			//cerr << error << endl;
			//Omega::printErrorLog(error);
		}
		return (error!=NULL);
	#endif
}


string DynLibManager::libNameToSystemName(const string& name)
{
	string systemName; 
	#ifdef WIN32
		systemName = name + ".dll";
	#else
		systemName = "lib" + name + ".so";
	#endif 
	return systemName;
}


string DynLibManager::systemNameToLibName(const string& name)
{
	string libName;

	#ifdef WIN32
		libName = name.substr(0,name.size()-4);
	#else
		libName = name.substr(3,name.size()-3);
	#endif 

	return libName;
}


string DynLibManager::findLibDir(const string& name)
{
	string libFileName = libNameToSystemName(name);

	string baseDir;
	baseDir.clear();

	vector<string>::iterator bdi    = baseDirs.begin();
	vector<string>::iterator bdiEnd = baseDirs.end();
	for( ; bdi != bdiEnd ; ++bdi)
	{
		filesystem::path name = filesystem::path((*bdi)+"/"+libFileName);
		if ( filesystem::exists( name ) )
		{
			baseDir = (*bdi);
			break;
		}
	}

	return baseDir;
}

